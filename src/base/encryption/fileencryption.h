#include <QObject>

#define KEY_SIZE 16

namespace Encryption {
    class Encryption{
    public:
        static void generateRandomKey(unsigned char *secretKey);
        static void encryptFile(QString filePath, unsigned char *secretKey, QString outputPath);
        static bool decryptFile(QString filePath, QString secretKey);

        static bool initializedEncryption;

    private:
        static void initEncryption();
        static unsigned char ASCIItoHex(char character);
    };
}
