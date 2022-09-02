#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line, data, total_memory, free_memory;
  std::istringstream linestream;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    linestream.str(line);
    linestream >> data >> total_memory >> data;
    std::getline(stream, line);
    linestream.clear();
    linestream.str(line);
    linestream >> data >> free_memory >> data;
  }
  if (total_memory != "" && free_memory != "") {
    return (float)(std::stol(total_memory) - std::stol(free_memory))/(float)std::stol(total_memory);
  }
  
 }

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line, system, process;
  std::istringstream linestream;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    linestream.str(line);
    linestream >> system >> process;
  }
  if (system != "" ) {return std::stol(system);}
  return 0;
 }

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  vector<string> jiffies = CpuUtilization();
  long total{0};
  for(string jiffie : jiffies) {
    total += std::stol(jiffie);
  }
  return total;
 }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string line, value;
  vector<string> values;
  std::istringstream linestream;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  if(stream.is_open()) {
    std::getline(stream, line);
    linestream.str(line);
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  if(values[13] != "" && values[14] != "") {return std::stol(values[13]) + std::stol(values[14]);}
  else return 0;
 }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> jiffies = CpuUtilization();
  return std::stol(jiffies[0]) + std::stol(jiffies[1]) + std::stol(jiffies[2])+ 
         std::stol(jiffies[5]) + std::stol(jiffies[6]) + std::stol(jiffies[7]);
 }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> jiffies = CpuUtilization();
  return std::stol(jiffies[3]) + std::stol(jiffies[4]);
 }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line, value, key;
  vector<string> values;
  std::istringstream linestream;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    linestream.str(line);
    linestream >> value;
    while (linestream >> value) {
      if (value != "") {values.push_back(value);}
    }
  }
  return values;
 }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line, value, key;
  std::istringstream linestream;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      linestream.clear();
      linestream.str(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          if (value != "") {return std::stoi(value); }
        }
      }
    }
  }
  return 0;
 }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line, value, key;
  std::istringstream linestream;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      linestream.clear();
      linestream.str(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          if (value != "") {return std::stoi(value); }
        }
      }
    }
  }
  return 0;
 }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;
  std::istringstream linestream;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  return line;
 }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line, key, value;
  std::istringstream linestream;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      linestream.clear();
      linestream.str(line);
      while(linestream >> key >> value) {
        if (key == "VmSize") {
          return value;
        }
      }
    }
  }
  return "Error";
 }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line, key, value;
  std::istringstream linestream;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      linestream.clear();
      linestream.str(line);
      while(linestream >> key >> value) {
        if (key == "Uid") {
          return value;
        }
      }
    }
  }
  return "Error";
 }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string userid = Uid(pid);
  string line, key, value, data;
  std::istringstream linestream;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      linestream.clear();
      linestream.str(line);
      while(linestream >> value >> data >> key) {
        if (key == userid) {
          return value;
        }
      }
    }
  }
  return value;
 }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string line, value;
  vector<string> values;
  std::istringstream linestream;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  if(stream.is_open()) {
    std::getline(stream, line);
    linestream.str(line);
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  if (values[21] != "")
    return std::stol(values[21]);
  else
    return 0;
 }

