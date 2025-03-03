#include <iostream>
#include <string>

std::string trim_whitespace(std::string input)
{
  std::string trimmed_input;

  size_t first = input.find_first_not_of(' ');

  if (std::string::npos == first)
  {
      return input;
  }

  size_t last = input.find_last_not_of(' ');
  trimmed_input =  input.substr(first, (last - first + 1));

  return trimmed_input;
}

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  while(true)
  {
      std::cout << "$ ";
      std::string input;
      std::getline(std::cin, input);

      // Trim input
      input = trim_whitespace(input);

      // Process input
      if(input == "exit 0")
      {
        exit(0);
      }
      else if(input=="echo" || input.find("echo ") != std::string::npos)
      {
        std::string echo_txt;

        try
        {
          echo_txt = input.substr(input.find("echo") + 5);
        }
        catch(const std::out_of_range& e)
        {
          echo_txt = "";
        }

        std::cout << echo_txt << "\n";
      }
      else
      {
        std::cout << input << ": command not found" << std::endl;
      }
  }
}
