
#include <QApplication>
#include <QLabel>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>
#include <QPixmap>
#include <opencv2/opencv.hpp>
#include <random>

class ImageEncryptionDecryption : public QMainWindow 
{
    Q_OBJECT

public:
    ImageEncryptionDecryption(QWidget* parent = nullptr) : QMainWindow(parent) 
    {
        setWindowTitle("Image Encryption Decryption");
        setFixedSize(1000, 700);

        // Create labels
        QLabel* titleLabel = new QLabel("Image Encryption\nDecryption");
        titleLabel->setFont(QFont("Arial", 40));
        titleLabel->setStyleSheet("color: magenta;");

        QLabel* originalLabel = new QLabel("Original\nImage");
        originalLabel->setFont(QFont("Arial", 40));
        originalLabel->setStyleSheet("color: magenta;");

        QLabel* editedLabel = new QLabel("Encrypted\nDecrypted\nImage");
        editedLabel->setFont(QFont("Arial", 40));
        editedLabel->setStyleSheet("color: magenta;");

        // Create buttons
        QPushButton* chooseButton = new QPushButton("Choose");
        chooseButton->setFont(QFont("Arial", 20));
        chooseButton->setStyleSheet("background-color: orange; color: blue; border: 3px solid;");

        QPushButton* saveButton = new QPushButton("Save");
        saveButton->setFont(QFont("Arial", 20));
        saveButton->setStyleSheet("background-color: orange; color: blue; border: 3px solid;");

        QPushButton* encryptButton = new QPushButton("Encrypt");
        encryptButton->setFont(QFont("Arial", 20));
        encryptButton->setStyleSheet("background-color: lightgreen; color: blue; border: 3px solid;");

        QPushButton* decryptButton = new QPushButton("Decrypt");
        decryptButton->setFont(QFont("Arial", 20));
        decryptButton->setStyleSheet("background-color: orange; color: blue; border: 3px solid;");

        QPushButton* resetButton = new QPushButton("Reset");
        resetButton->setFont(QFont("Arial", 20));
        resetButton->setStyleSheet("background-color: yellow; color: blue; border: 3px solid;");

        QPushButton* exitButton = new QPushButton("EXIT");
        exitButton->setFont(QFont("Arial", 20));
        exitButton->setStyleSheet("background-color: red; color: blue; border: 3px solid;");

        // Create image labels
        originalImageLabel = new QLabel;
        editedImageLabel = new QLabel;

        // Connect signals and slots
        connect(chooseButton, &QPushButton::clicked, this, &ImageEncryptionDecryption::openImage);
        connect(saveButton, &QPushButton::clicked, this, &ImageEncryptionDecryption::saveImage);
        connect(encryptButton, &QPushButton::clicked, this, &ImageEncryptionDecryption::encryptImage);
        connect(decryptButton, &QPushButton::clicked, this, &ImageEncryptionDecryption::decryptImage);
        connect(resetButton, &QPushButton::clicked, this, &ImageEncryptionDecryption::resetImage);
        connect(exitButton, &QPushButton::clicked, qApp, &QApplication::quit);

        // Create layouts
        QHBoxLayout* topLayout = new QHBoxLayout;
        topLayout->addWidget(chooseButton);
        topLayout->addWidget(saveButton);
        topLayout->addStretch();
        topLayout->addWidget(exitButton);

        QHBoxLayout* imageLayout = new QHBoxLayout;
        imageLayout->addWidget(originalImageLabel);
        imageLayout->addWidget(editedImageLabel);

        QHBoxLayout* buttonLayout = new QHBoxLayout;
        buttonLayout->addStretch();
        buttonLayout->addWidget(encryptButton);
        buttonLayout->addWidget(decryptButton);
        buttonLayout->addWidget(resetButton);
        buttonLayout->addStretch();

        QVBoxLayout* mainLayout = new QVBoxLayout;
        mainLayout->addWidget(titleLabel, 0, Qt::AlignHCenter);
        mainLayout->addLayout(topLayout);
        mainLayout->addWidget(originalLabel, 0, Qt::AlignHCenter);
        mainLayout->addWidget(editedLabel, 0, Qt::AlignHCenter);
        mainLayout->addLayout(imageLayout);
        mainLayout->addLayout(buttonLayout);

        QWidget* centralWidget = new QWidget;
        centralWidget->setLayout(mainLayout);
        setCentralWidget(centralWidget);
    }

private slots:
    void openImage() 
    {
        QString fileName = QFileDialog::getOpenFileName(this, "Open Image", QDir::currentPath(), "Image Files (*.png *.jpg *.bmp)");
        if (!fileName.isEmpty()) 
        {
            originalImage = QImage(fileName);
            editedImage = originalImage;
            updateImageLabels();
        }
    }

    void saveImage() 
    {
        QString fileName = QFileDialog::getSaveFileName(this, "Save Image", QDir::currentPath(), "Image Files (*.png *.jpg *.bmp)");
        if (!fileName.isEmpty()) 
        {
            editedImage.save(fileName);
            QMessageBox::information(this, "Success", "Encrypted Image Saved Successfully!");
        }
    }

    void encryptImage() 
    {
        if (originalImage.isNull()) 
        {
            QMessageBox::warning(this, "Error", "Please open an image first.");
            return;
        }

        cv::Mat inputImage = cv::Mat(originalImage.height(), originalImage.width(), CV_8UC3, originalImage.bits(), originalImage.bytesPerLine()).clone();
        cv::Mat grayImage;
        cv::cvtColor(inputImage, grayImage, cv::COLOR_BGR2GRAY);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<> dis(0.0, 0.1);

        cv::Mat key(grayImage.rows, grayImage.cols, CV_64FC1);
        for (int i = 0; i < key.rows; ++i) 
        {
            for (int j = 0; j < key.cols; ++j) 
            {
                key.at<double>(i, j) = dis(gen) + std::numeric_limits<double>::epsilon();
            }
        }

        cv::Mat encryptedImage = grayImage / key;
        encryptedImage.convertTo(encryptedImage, CV_8UC1, 255.0);

        QImage qEncryptedImage = QImage(encryptedImage.data, encryptedImage.cols, encryptedImage.rows, encryptedImage.step, QImage::Format_Grayscale8);
        editedImage = qEncryptedImage;
        updateImageLabels();

        QMessageBox::information(this, "Encrypt Status", "Image Encrypted successfully.");
    }

    void decryptImage() 
    {
        if (editedImage.isNull()) 
        {
            QMessageBox::warning(this, "Error", "Please encrypt an image first.");
            return;
        }

        cv::Mat encryptedImage = cv::Mat(editedImage.height(), editedImage.width(), CV_8UC1, editedImage.bits(), editedImage.bytesPerLine()).clone();
        cv::Mat decryptedImage = encryptedImage.clone();

        for (int i = 0; i < decryptedImage.rows; ++i) 
        {
            for (int j = 0; j < decryptedImage.cols; ++j) 
            {
                decryptedImage.at<uchar>(i, j) = static_cast<uchar>(encryptedImage.at<uchar>(i, j) * key.at<double>(i, j));
            }
        }

        QImage qDecryptedImage = QImage(decryptedImage.data, decryptedImage.cols, decryptedImage.rows, decryptedImage.step, QImage::Format_Grayscale8);
        editedImage = qDecryptedImage;
        updateImageLabels();

        QMessageBox::information(this, "Decrypt Status", "Image decrypted successfully.");
    }

    void resetImage() 
    {
        if (originalImage.isNull()) 
        {
            QMessageBox::warning(this, "Error", "Please open an image first.");
            return;
        }

        editedImage = originalImage;
        updateImageLabels();
        QMessageBox::information(this, "Success", "Image reset to original format!");
    }

private:
    void updateImageLabels() 
    {
        originalImageLabel->setPixmap(QPixmap::fromImage(originalImage));
        editedImageLabel->setPixmap(QPixmap::fromImage(editedImage));
    }

    QImage originalImage;
    QImage editedImage;
    cv::Mat key;
    QLabel* originalImageLabel;
    QLabel* editedImageLabel;
};

int main(int argc, char* argv[]) 
{
    QApplication app(argc, argv);
    ImageEncryptionDecryption window;
    window.show();
    return app.exec();
}

This C++ code uses the Qt framework for creating the GUI and OpenCV for image processing. It provides functionality to open, encrypt, decrypt, reset, and save images. The encryption is performed using a random key generated from a normal distribution. The key is used to divide the grayscale image pixel values for encryption, and the encrypted image is multiplied by the key for decryption.

Note: Make sure to include the required Qt and OpenCV libraries when compiling this code.

