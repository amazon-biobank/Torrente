#include <QObject>
#include <QFile>
#include <QDir>

#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include "fileencryption.h"

#define ENCRYPTION_BUFFER_SIZE 64
#define HEX_SIZE 16

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

void Encryption::Encryption::encryptFile(QString filePath, unsigned char* secretKey, QString outputPath) {
    initEncryption();

    unsigned char fileTag[AES_BLOCK_SIZE];
    QDir dir(QFileInfo(outputPath).dir());
    if (!dir.exists())
        dir.mkpath(".");

    // First, i need to get files references;
    FILE* fileToEncrypt = fopen(filePath.toStdString().c_str(), "rb");
    FILE* fileEncrypted = fopen(outputPath.toStdString().c_str(), "wb");

    unsigned char iv[AES_BLOCK_SIZE];
    unsigned char key[AES_BLOCK_SIZE];
    std::copy(secretKey, secretKey + AES_BLOCK_SIZE, iv);                           // The secret key first half will be IV
    std::copy(secretKey + AES_BLOCK_SIZE, secretKey + (AES_BLOCK_SIZE * 2), key);   // The secret key second half

    EVP_CIPHER_CTX* e_ctx = EVP_CIPHER_CTX_new();           // Create context placeholder;
    EVP_EncryptInit(e_ctx, EVP_aes_128_gcm(), key, iv);     // Initialize encryption context with key and IV;

    unsigned char encryptionBuffer[ENCRYPTION_BUFFER_SIZE];
    unsigned char bufferToEncrypt[ENCRYPTION_BUFFER_SIZE];
    int numBytesRead, outputLength;
    while (true) {
        numBytesRead = fread(bufferToEncrypt, sizeof(unsigned char), ENCRYPTION_BUFFER_SIZE, fileToEncrypt);
        EVP_EncryptUpdate(e_ctx, encryptionBuffer, &outputLength, bufferToEncrypt, ENCRYPTION_BUFFER_SIZE);
        fwrite(encryptionBuffer, sizeof(unsigned char), outputLength, fileEncrypted);
        if (numBytesRead < ENCRYPTION_BUFFER_SIZE) {
            /* Reached End of file */
            break;
        }
    }

    EVP_EncryptFinal(e_ctx, encryptionBuffer, &outputLength); // Append finalizing encryption buffer;
    fwrite(encryptionBuffer, sizeof(unsigned char), outputLength, fileEncrypted); // Stores trailing buffer to output file;
    EVP_CIPHER_CTX_ctrl(e_ctx, EVP_CTRL_GCM_GET_TAG, AES_BLOCK_SIZE, fileTag);  // Generates file tag;
    fwrite(fileTag, sizeof(unsigned char), AES_BLOCK_SIZE, fileEncrypted); // Stores trailing buffer to output file;
    EVP_CIPHER_CTX_free(e_ctx); // Free memory alocation;
    fclose(fileEncrypted);
    fclose(fileToEncrypt);
}

bool Encryption::Encryption::decryptFile(QString filePath, QString secretKey) {
    initEncryption();

    // First, i need to get files references;
    FILE* fileToDecrypt = fopen(filePath.toStdString().c_str(), "rb");

    // And then, the output reference;
    int indexOfCypherExtension = filePath.lastIndexOf(QChar('.'));
    QString outputPath = filePath.left(indexOfCypherExtension);
    FILE* fileDecrypted = fopen(outputPath.toStdString().c_str(), "wb");

    unsigned char tag[AES_BLOCK_SIZE];
    unsigned char key[AES_BLOCK_SIZE];
    unsigned char iv[AES_BLOCK_SIZE];
    std::string keyString = secretKey.toStdString();
    for (int i = 0; i < keyString.length() / 4; i++)
    {
        iv[i] = (
            Encryption::ASCIItoHex(keyString[i * 2]) * HEX_SIZE +
            Encryption::ASCIItoHex(keyString[(i * 2) + 1])
            );
        key[i] = (
            Encryption::ASCIItoHex(keyString[i * 2 + keyString.length() / 2]) * HEX_SIZE +
            Encryption::ASCIItoHex(keyString[(i * 2) + 1 + keyString.length() / 2])
            );
    }

    EVP_CIPHER_CTX* d_ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit(d_ctx, EVP_aes_128_gcm(), key, iv);

    unsigned char decryptionBuffer[ENCRYPTION_BUFFER_SIZE];
    unsigned char bufferToDecrypt[ENCRYPTION_BUFFER_SIZE];
    int numBytesRead, outputLength;
    while (true) {
        numBytesRead = fread(bufferToDecrypt, sizeof(unsigned char), ENCRYPTION_BUFFER_SIZE, fileToDecrypt);
        if (numBytesRead < ENCRYPTION_BUFFER_SIZE) {
            std::copy(bufferToDecrypt, bufferToDecrypt + AES_BLOCK_SIZE, tag);
            /* Reached End of file */
            break;
        }
        EVP_DecryptUpdate(d_ctx, decryptionBuffer, &outputLength, bufferToDecrypt, ENCRYPTION_BUFFER_SIZE); // Cypher block and store in buffer
        fwrite(decryptionBuffer, sizeof(unsigned char), outputLength, fileDecrypted); // Stores encrypted block to output file;
    }
    EVP_CIPHER_CTX_ctrl(d_ctx, EVP_CTRL_GCM_SET_TAG, AES_BLOCK_SIZE, tag);
    if (EVP_DecryptFinal(d_ctx, decryptionBuffer, &outputLength) == 0) {
        fclose(fileDecrypted);
        fclose(fileToDecrypt);
        return false;
    }
    EVP_CIPHER_CTX_free(d_ctx);
    fclose(fileDecrypted);
    fclose(fileToDecrypt);
    return true;
}

unsigned char Encryption::Encryption::ASCIItoHex(char character) {
    if (character >= 'A' && character <= 'F')
        return character - 55;
    else if (character >= '0' && character <= '9')
        return character - 48;
    else
        return 0;
}
