#include "SPIFFS.h"

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    MySerial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        MySerial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        MySerial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            MySerial.print("  DIR : ");
            MySerial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            MySerial.print("  FILE: ");
            MySerial.print(file.name());
            MySerial.print("\tSIZE: ");
            MySerial.println(file.size());
        }
        file = root.openNextFile();
    }
}

String readFile(fs::FS &fs, const char * path){
      String content = "";
    MySerial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        MySerial.println("- failed to open file for reading");
        
    }else{
      MySerial.println("- read from file:");
      while(file.available()){
          char c = (char)file.read();
          content += c;
      }
          MySerial.print(content);
    }
    file.close();
    return content;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    MySerial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        MySerial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        MySerial.println("- file written");
    } else {
        MySerial.println("- write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    MySerial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        MySerial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        MySerial.println("- message appended");
    } else {
        MySerial.println("- append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    MySerial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        MySerial.println("- file renamed");
    } else {
        MySerial.println("- rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    MySerial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        MySerial.println("- file deleted");
    } else {
        MySerial.println("- delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    MySerial.printf("Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        MySerial.println("- failed to open file for writing");
        return;
    }

    size_t i;
    MySerial.print("- writing" );
    uint32_t start = millis();
    for(i=0; i<2048; i++){
        if ((i & 0x001F) == 0x001F){
          MySerial.print(".");
        }
        file.write(buf, 512);
    }
    MySerial.println("");
    uint32_t end = millis() - start;
    MySerial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
    file.close();

    file = fs.open(path);
    start = millis();
    end = start;
    i = 0;
    if(file && !file.isDirectory()){
        len = file.size();
        size_t flen = len;
        start = millis();
        MySerial.print("- reading" );
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F){
              MySerial.print(".");
            }
            len -= toRead;
        }
        MySerial.println("");
        end = millis() - start;
        MySerial.printf("- %u bytes read in %u ms\r\n", flen, end);
        file.close();
    } else {
        MySerial.println("- failed to open file for reading");
    }
}
