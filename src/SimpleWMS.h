/**
 * Copyright (c) 2020. <ADD YOUR HEADER INFORMATION>.
 * Generated with the wrench-init.in tool.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */
#ifndef MY_SIMPLEWMS_H
#define MY_SIMPLEWMS_H

#include <wrench-dev.h>

class Simulation;

/**
 *  @brief A simple WMS implementation
 */
class SimpleWMS : public wrench::WMS {
public:
    SimpleWMS(const std::set<std::shared_ptr<wrench::StorageService>> &storage_services,
              const std::string &hostname);

private:
    int main() override;

};

#endif //MY_SIMPLEWMS_H

