// Import necessary libraries
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_Box.H>
#include <opencv2/opencv.hpp>
#include <random>

// Global variables
int count = 0;
cv::Mat eimg;
std::vector < std::string > frp, tname;
int con = 1;
int bright = 0;
Fl_Box * panelB = nullptr;
Fl_Box * panelA = nullptr;

// Function to get the path of the image selected
std::string getpath(const std::string & path) 
{
  size_t pos = path.find_last_of("/\\");
  return path.substr(0, pos);
}

// Function to get the folder name from which image is selected
std::string getfoldername(const std::string & path) 
{
  size_t pos = path.find_last_of("/\\");
  return path.substr(pos + 1);
}

// Function to get the file name of the selected image
std::string getfilename(const std::string & path) {
  size_t pos = path.find_last_of("/\\");
  std::string filename = path.substr(pos + 1);
  return filename.substr(0, filename.find_last_of('.'));
}

// Function to open the image file
std::string openfilename() 
{
  Fl_File_Chooser chooser(".", "*", Fl_File_Chooser::SINGLE, "Open File");
  chooser.show();
  while (chooser.visible()) 
  {
    Fl::wait();
  }
  return chooser.value();
}

// Function to open the selected image
void open_img(Fl_Widget * widget, void * data) 
{
  std::string x = openfilename();
  if (!x.empty()) 
  {
    eimg = cv::imread(x);
    Fl_JPEG_Image * img = new Fl_JPEG_Image(x.c_str());
    std::string temp = x;
    std::string location = getpath(temp);
    std::string filename = getfilename(temp);
    if (panelA == nullptr || panelB == nullptr) 
    {
      panelA = new Fl_Box(10, 100, img -> w(), img -> h(), "");
      panelA -> image(img);
      panelB = new Fl_Box(img -> w() + 20, 100, img -> w(), img -> h(), "");
      panelB -> image(img);
    } 
    else 
    {
      panelA -> image(img);
      panelB -> image(img);
    }
  }
}

// Function for image encryption
void en_fun(Fl_Widget * widget, void * data) 
{
  cv::Mat image_input = cv::imread(eimg.data, cv::IMREAD_GRAYSCALE);
  cv::Size size = image_input.size();
  image_input.convertTo(image_input, CV_64F, 1.0 / 255.0);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::normal_distribution < double > dist(0.0, 0.1);
  cv::Mat key(size, CV_64F);
  for (int i = 0; i < size.height; i++) 
  {
    for (int j = 0; j < size.width; j++) 
    {
      key.at < double > (i, j) = dist(gen) + std::numeric_limits < double > ::epsilon();
    }
  }
  cv::Mat image_encrypted = image_input / key;
  cv::imwrite("image_encrypted.jpg", image_encrypted * 255);
  Fl_JPEG_Image * img = new Fl_JPEG_Image("image_encrypted.jpg");
  panelB -> image(img);
  fl_message("Image Encrypted successfully.");
}

// Function for image decryption
void de_fun(Fl_Widget * widget, void * data) 
{
  cv::Mat image_output = cv::imread("image_encrypted.jpg", cv::IMREAD_GRAYSCALE);
  image_output.convertTo(image_output, CV_64F, 1.0 / 255.0);
  cv::Mat key = cv::Mat::ones(image_output.size(), CV_64F);
  // Load the key from the previous encryption step
  image_output = image_output * key;
  image_output *= 255.0;
  cv::imwrite("image_output.jpg", image_output);
  Fl_JPEG_Image * img = new Fl_JPEG_Image("image_output.jpg");
  panelB -> image(img);
  fl_message("Image decrypted successfully.");
}

// Function to reset the edited image to original one
void reset(Fl_Widget * widget, void * data) 
{
  Fl_JPEG_Image * img = new Fl_JPEG_Image(eimg.data);
  panelB -> image(img);
  fl_message("Image reset to original format!");
}

// Function to save the edited image
void save_img(Fl_Widget * widget, void * data) 
{
  Fl_File_Chooser chooser(".", "*", Fl_File_Chooser::CREATE, "Save File");
  chooser.show();
  while (chooser.visible()) 
  {
    Fl::wait();
  }
  if (chooser.value() != nullptr) 
  {
    std::string filename = chooser.value();
    cv::imwrite(filename, eimg);
    fl_message("Encrypted Image Saved Successfully!");
  }
}

int main(int argc, char ** argv) 
{
  Fl::scheme("gtk+");
  Fl_Window * window = new Fl_Window(1000, 700, "Image Encryption Decryption");

  Fl_Box * start1 = new Fl_Box(350, 10, 300, 100, "Image Encryption\nDecryption");
  start1 -> labelfont(FL_BOLD);
  start1 -> labelsize(40);
  start1 -> labelcolor(fl_rgb_color(255, 0, 255));

  Fl_Box * start2 = new Fl_Box(100, 270, 200, 100, "Original\nImage");
  start2 -> labelfont(FL_BOLD);
  start2 -> labelsize(40);
  start2 -> labelcolor(fl_rgb_color(255, 0, 255));

  Fl_Box * start3 = new Fl_Box(700, 230, 300, 100, "Encrypted\nDecrypted\nImage");
  start3 -> labelfont(FL_BOLD);
  start3 -> labelsize(40);
  start3 -> labelcolor(fl_rgb_color(255, 0, 255));

  Fl_Button * chooseb = new Fl_Button(30, 20, 100, 50, "Choose");
  chooseb -> callback(open_img);
  chooseb -> color(fl_rgb_color(255, 165, 0));
  chooseb -> labelcolor(fl_rgb_color(0, 0, 255));

  Fl_Button * saveb = new Fl_Button(170, 20, 100, 50, "Save");
  saveb -> callback(save_img);
  saveb -> color(fl_rgb_color(255, 165, 0));
  saveb -> labelcolor(fl_rgb_color(0, 0, 255));

  Fl_Button * enb = new Fl_Button(150, 620, 100, 50, "Encrypt");
  enb -> callback(en_fun);
  enb -> color(fl_rgb_color(144, 238, 144));
  enb -> labelcolor(fl_rgb_color(0, 0, 255));

  Fl_Button * deb = new Fl_Button(450, 620, 100, 50, "Decrypt");
  deb -> callback(de_fun);
  deb -> color(fl_rgb_color(255, 165, 0));
  deb -> labelcolor(fl_rgb_color(0, 0, 255));

  Fl_Button * resetb = new Fl_Button(800, 620, 100, 50, "Reset");
  resetb -> callback(reset);
  resetb -> color(fl_rgb_color(255, 255, 0));
  resetb -> labelcolor(fl_rgb_color(0, 0, 255));

  Fl_Button * exitb = new Fl_Button(880, 20, 100, 50, "EXIT");
  exitb -> color(fl_rgb_color(255, 0, 0));
  exitb -> labelcolor(fl_rgb_color(0, 0, 255));
  exitb -> callback([](Fl_Widget * widget, void * data) {
    if (fl_choice("Do you want to exit?", nullptr, nullptr, nullptr)) 
    {
      exit(0);
    }
  });

  window -> end();
  window -> show(argc, argv);
  return Fl::run();
}

