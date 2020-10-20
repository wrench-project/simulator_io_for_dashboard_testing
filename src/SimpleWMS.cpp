/**
 * Copyright (c) 2020. <ADD YOUR HEADER INFORMATION>.
 * Generated with the wrench-init.in tool.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */
#include <iostream>

#include "SimpleWMS.h"

#define NUM_FILES 10
#define MB (1000.0*1000.0)

XBT_LOG_NEW_DEFAULT_CATEGORY(simple_wms, "Log category for Simple WMS");

/**
 * @brief Create a Simple WMS with a workflow instance, a scheduler implementation, and a list of compute services
 */
SimpleWMS::SimpleWMS(const std::set<std::shared_ptr<wrench::StorageService>> &storage_services,
                     const std::string &hostname) : wrench::WMS(
        nullptr,
        nullptr,
        {},
        storage_services,
        {},
        nullptr,
        hostname,
        "simple") {}

/**
 * @brief main method of the SimpleWMS daemon
 */
int SimpleWMS::main() {

    wrench::TerminalOutput::setThisProcessLoggingColor(wrench::TerminalOutput::COLOR_GREEN);

    // Check whether the WMS has a deferred start time
    checkDeferredStart();

    std::vector<std::shared_ptr<wrench::StorageService>> ss;
    for (auto const &s : this->getAvailableStorageServices()) {
        ss.push_back(s);
    }

    // Create a data movement manager
    std::shared_ptr<wrench::DataMovementManager> data_movement_manager = this->createDataMovementManager();

    // Create a bunch of files and write them all to servers 0 to 4
    std::vector<wrench::WorkflowFile *> files;
    for (int i=0; i < NUM_FILES; i++) {
        // Create a new workflow file
        auto file = this->getWorkflow()->addFile("file_"+std::to_string(i), 10.0*(i+1)*MB);
        files.push_back(file);
        for (int j=0; j < 5;j++) {
            ss.at(j)->writeFile(file, wrench::FileLocation::LOCATION(ss.at(j)));
        }
    }

    std::vector<std::vector<std::tuple<int,int,int>>> file_operations;
    file_operations.push_back({{0,0,5}});
    file_operations.push_back({{1,0,5},{1,0,6}});
    file_operations.push_back({{2,0,5},{2,0,6},{2,0,7}});
    file_operations.push_back({{3,0,5},{4,1,5},{5,2,5}});
    file_operations.push_back({{6,0,5},{7,1,5},{8,2,5},{3,5,6},{4,5,6},{5,5,7}});

    for (auto const &opset : file_operations) {
        WRENCH_INFO("===== FILE WRITE EXPERIMENT ====");
        for (auto const &op: opset) {
            auto file = files.at(std::get<0>(op));
            auto src = ss.at(std::get<1>(op));
            auto dst = ss.at(std::get<2>(op));
            WRENCH_INFO("Initiating file transfer: %s:%s->%s (%.1lf MB)",
                    file->getID().c_str(),
                    src->getHostname().c_str(),
                    dst->getHostname().c_str(),
                        (file->getSize()/MB));
            data_movement_manager->initiateAsynchronousFileCopy(file, wrench::FileLocation::LOCATION(src), wrench::FileLocation::LOCATION(dst));
        }
        for (auto const &op: opset) {
            try {
                this->waitForAndProcessNextEvent();
            } catch (wrench::WorkflowExecutionException &e) {
                WRENCH_INFO("Error while getting next execution event (%s)... shouldn't be happening",
                            (e.getCause()->toString().c_str()));
                exit(0);
            }
        }
    }


#if 0
    // Wait for a workflow execution event, and process it
    try {
        this->waitForAndProcessNextEvent();
    } catch (wrench::WorkflowExecutionException &e) {
        WRENCH_INFO("Error while getting next execution event (%s)... ignoring and trying again",
                    (e.getCause()->toString().c_str()));
        continue;
    }

    if (this->getWorkflow()->isDone()) {
        break;
    }
#endif

    wrench::Simulation::sleep(10);

    return 0;
}
