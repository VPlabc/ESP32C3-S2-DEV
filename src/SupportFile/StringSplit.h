#include <Arduino.h>

String strs[20];
int StringCount = 0;
String checkLine(String data, String split){
 while (data.length() > 0)
  {
    int index = data.indexOf(split);
    if (index == -1 ) // No space found
    {
      strs[StringCount++] = data;
      break;
    }
    else
    {
      strs[StringCount++] = data.substring(1, index);
      data = data.substring(index+1);
    }
  }
  String StringData ="";
  if(StringCount > 0){StringData = strs[1];
  StringCount = 0;
  int index = data.indexOf("\"");
  StringData = StringData.substring(1, index);}
  return StringData;
}