#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    cout << Usage: ./rootkit [command] [arguments] << endl;
    return 1;
  }

  string command = argv[1];
  vector<string> args;
  for (int i = 2; i < argc; ++i)
  {
    args.push_back(argv[i]);
  }

  if (command == list)
  {
    cout << Listing files... << endl;
    system(ls);
  }
  else if (command == rm)
  {
    cout << Deleting files... << endl;
    for (int i = 0; i < args.size(); ++i)
    {
      string command = string(argv[i]);
      system(rm command);
    }
  }
  else
  {
    cout << Invalid command. List or rm only. << endl;
  }

  return 0;
}
