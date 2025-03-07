#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <set>
#include <iterator>
#include <cstdlib>
#include <filesystem>

std::vector<std::string> split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

bool find_executable(std::string command, std::string dir)
{
    for (const auto& entry : std::filesystem::directory_iterator(dir))
    {
        if (command == entry.path().filename())
        {
            return true;
        }
    }
  return false;
}

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // Get PATH
  const char* path_var = std::getenv("PATH");
  std::vector <std::string> path_dirs = split(path_var,':');

  std::map<std::string, std::string> builtins = 
  {
    {"echo", "echo is a shell builtin"},
    {"type", "type is a shell builtin"},
    {"exit", "exit is a shell builtin"}
  };

  while(true)
  {
      std::cout << "$ ";
      std::string input;
      std::getline(std::cin, input);

      // Split input string based on whitespace
      std::istringstream iss(input);
      std::vector<std::string> split_input(std::istream_iterator<std::string>{iss},
      std::istream_iterator<std::string>());

      std::string command;
      std::vector<std::string> args;

      // if input is not empty
      if(split_input.size() != 0)
      {
        // Extract command and arguments
        command = split_input[0];
        args.insert(args.end(),split_input.begin()+1,split_input.end());

        // Check if command exists
        if(builtins.count(command) == 0)
        {
          std::cout<<command<<": command not found\n";
        }
        else
        {
          // Exit
          if (command == "exit")
          {
            exit(std::stoi(args[0]));
          }
          // echo
          else if (command == "echo")
          {
            for(const auto &arg:args)
            {
              std::cout<<arg<<" ";
            }
          }
          // type
          else if (command == "type")
          {
            // Find command
            for(const auto &arg:args)
            {
              // First check builtins
              if(builtins.count(arg)!=0)
              {
                std::cout<< builtins[arg];
              }
              else
              {
                // Search in PATH
                bool found = false;
                for(const auto &dir:path_dirs)
                {
                  found = find_executable(arg,dir);
                  if(found)
                  {
                    std::cout<<arg<<" is "<< std::filesystem::path(dir).append(arg).string();
                    break;
                  }
                }

                if(!found)
                {
                  std::cout<<arg<<": not found";
                }
              }              
            }       
          }
          std::cout<<"\n";
        }
      }
      else
      {
        std::cout<<"\n";
      }
      
  }
}
