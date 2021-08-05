#include <QObject>
#include <QFile>

#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include "fileencryption.h"

#define ENCRYPTION_BUFFER_SIZE 64

bool Encryption::Encryption::initializedEncryption = false;

void Encryption::Encryption::initEncryption() {
    if (Encryption::initializedEncryption) {
        //initialize openssl ciphers
        OpenSSL_add_all_ciphers();

        //initialize random number generator (for IVs)
        RAND_poll();

        Encryption::initializedEncryption = true;
    }
}

void Encryption::Encryption::generateRandomKey(unsigned char* randomKey) {
    initEncryption();
    int result = RAND_bytes(randomKey, KEY_SIZE * 2);
    randomKey[KEY_SIZE * 2] = '\0';
}

void Encryption::Encryption::encryptFile(QString filePath, unsigned char* secretKey, QString outputPath, unsigned char* fileTag) {
    initEncryption();

    // First, i need to get file reference;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    // And then, the output reference;
    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    unsigned char iv[AES_BLOCK_SIZE];
    unsigned char key[AES_BLOCK_SIZE];
    std::copy(secretKey, secretKey + AES_BLOCK_SIZE, iv);                           // The secret key first half will be IV
    std::copy(secretKey + AES_BLOCK_SIZE, secretKey + (AES_BLOCK_SIZE * 2), key);   // The secret key second hald 

    int actual_size = 0, final_size = 0;
    EVP_CIPHER_CTX* e_ctx = EVP_CIPHER_CTX_new();           // Create context placeholder;
    EVP_EncryptInit(e_ctx, EVP_aes_128_gcm(), key, iv);     // Initialize encryption context with key and IV;

    unsigned char encryptionBuffer[ENCRYPTION_BUFFER_SIZE];
    QByteArray blockToCypher, cypheredBlock;
    while (!file.atEnd()) {
        blockToCypher = file.read(ENCRYPTION_BUFFER_SIZE);  // Read 64 bytes from file to cypher
        EVP_EncryptUpdate(e_ctx, encryptionBuffer, &actual_size, (const unsigned char*)blockToCypher.constData(), ENCRYPTION_BUFFER_SIZE); // Cypher block and store in buffer
        cypheredBlock = QByteArray(reinterpret_cast<char*>(encryptionBuffer), ENCRYPTION_BUFFER_SIZE); // Transform cyphered block in QByteArray
        outputFile.write(cypheredBlock); // Stores cyphered block to output file;
    }

    EVP_EncryptFinal(e_ctx, encryptionBuffer, &final_size); // Append finalizing encryption buffer;
    cypheredBlock = QByteArray(reinterpret_cast<char*>(encryptionBuffer), final_size); // Transform cyphered block in QByteArray
    outputFile.write(cypheredBlock); // Stores trailing buffer to output file;
    EVP_CIPHER_CTX_ctrl(e_ctx, EVP_CTRL_GCM_GET_TAG, AES_BLOCK_SIZE, fileTag);  // Generates file tag;
    EVP_CIPHER_CTX_free(e_ctx); // Free memory alocation;
    outputFile.close();
    file.close();
}

void Encryption::Encryption::decryptFile(QString filePath, QString secretKey, QString fileTag) {
    initEncryption();

    // First, i need to get file reference;
    QFile fileToDecypher(filePath);
    if (!fileToDecypher.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    // And then, the output reference;
    int indexOfCypherExtension = filePath.lastIndexOf(QChar('.'));
    QString outputPath = filePath.left(indexOfCypherExtension);
    QFile decypheredOutputFile(outputPath);
    if (!decypheredOutputFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    unsigned char tag[AES_BLOCK_SIZE];
    unsigned char key[AES_BLOCK_SIZE];
    unsigned char iv[AES_BLOCK_SIZE];
    for (int i = 0; i < secretKey.length() / 2; i++)
    {
        key[i] = secretKey[i * 2].digitValue() * KEY_SIZE + secretKey[(i * 2) + 1].digitValue();
        iv[i] = secretKey[i * 2 + KEY_SIZE].digitValue() * KEY_SIZE + secretKey[(i * 2) + 1 + KEY_SIZE].digitValue();
    }
    for (int i = 0; i < fileTag.length() / 2; i += 2)
    {
        tag[i] = fileTag[i].digitValue() * KEY_SIZE + fileTag[i + 1].digitValue();
    }

    int actual_size = 0, final_size = 0;
    EVP_CIPHER_CTX* d_ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit(d_ctx, EVP_aes_128_gcm(), key, iv);

    unsigned char decryptionBuffer[ENCRYPTION_BUFFER_SIZE];
    QByteArray blockToDecypher, decypheredBlock;
    while (!fileToDecypher.atEnd()) {
        blockToDecypher = fileToDecypher.read(ENCRYPTION_BUFFER_SIZE);  // Read 64 bytes from file to cypher
        EVP_DecryptUpdate(d_ctx, decryptionBuffer, &actual_size, (const unsigned char*) blockToDecypher.constData(), ENCRYPTION_BUFFER_SIZE); // Cypher block and store in buffer
        decypheredBlock = QByteArray(reinterpret_cast<char*>(decryptionBuffer), ENCRYPTION_BUFFER_SIZE); // Transform cyphered block in QByteArray
        decypheredOutputFile.write(decypheredBlock); // Stores cyphered block to output file;
    }
    EVP_CIPHER_CTX_ctrl(d_ctx, EVP_CTRL_GCM_SET_TAG, AES_BLOCK_SIZE, tag);
    EVP_DecryptFinal(d_ctx, decryptionBuffer, &final_size);
    decypheredBlock = QByteArray(reinterpret_cast<char*>(decryptionBuffer), final_size); // Transform cyphered block in QByteArray
    decypheredOutputFile.write(decypheredBlock); // Stores trailing buffer to output file;
    EVP_CIPHER_CTX_free(d_ctx);
    decypheredOutputFile.close();
    fileToDecypher.close();
}
