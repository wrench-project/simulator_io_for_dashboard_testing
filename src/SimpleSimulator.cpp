/**
 * Copyright (c) 2020. <ADD YOUR HEADER INFORMATION>.
 * Generated with the wrench-init.in tool.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */
#include <wrench.h>
#include "SimpleWMS.h"

void generate_platform_file(std::string file_path) {
    // Create a platform file
    std::string xml = "<?xml version='1.0'?>\n"
                      "<!DOCTYPE platform SYSTEM \"http://simgrid.gforge.inria.fr/simgrid/simgrid.dtd\">\n"
                      "<platform version=\"4.1\">\n"
                      "   <zone id=\"AS0\" routing=\"Full\">\n";
    for (int i=0; i < 10; i++) {
        xml += ""
               "       <host id=\"Host_" + std::to_string(i) + "\" speed=\"1f\" core=\"2\"> \n"
                                                               "          <disk id=\"disk_" + std::to_string(i) +
               "\" read_bw=\"10MBps\" write_bw=\"10MBps\">\n"
               "             <prop id=\"size\" value=\"3000GB\"/>\n"
               "             <prop id=\"mount\" value=\"/\"/>\n"
               "          </disk>\n"
               "       </host>\n";
    }
    xml += "       <link id=\"link\" bandwidth=\"5000GBps\" latency=\"0us\"/>\n";

    for (int i=0; i < 10; i++) {
        for (int j=i+1; j <10; j++) {
            xml += "  <route src=\"Host_"+std::to_string(i)+"\" dst=\"Host_"+std::to_string(j)+
                   "\"> <link_ctn id=\"link\"/> </route>\n";
        }
    }

    xml += "   </zone> \n"
           "</platform>\n";
    FILE *platform_file = fopen(file_path.c_str(), "w");
    fprintf(platform_file, "%s", xml.c_str());
    fclose(platform_file);
}

int main(int argc, char **argv) {

    // Declaration of the top-level WRENCH simulation object
    wrench::Simulation simulation;

    // Initialization of the simulation
    simulation.init(&argc, argv);
    simulation.getOutput().enableDiskTimestamps(true);
    auto workflow = new wrench::Workflow();

    // Reading and parsing the platform description file to instantiate a simulated platform
    generate_platform_file("/tmp/platform.xml");
    simulation.instantiatePlatform("/tmp/platform.xml");

    // Get a vector of all the hosts in the simulated platform
    std::vector<std::string> hostname_list = wrench::Simulation::getHostnameList();

    // Create storage services
    std::set<std::shared_ptr<wrench::StorageService>> storage_services;
    for (auto const &h : hostname_list) {
        // Instantiate a storage service on each host in the platform
        std::cerr << "Instantiating a SimpleStorageService on " << h << "..." << std::endl;
        auto storage_service = simulation.add(new wrench::SimpleStorageService(h, {"/"},
                                                                               {{wrench::SimpleStorageServiceProperty::BUFFER_SIZE,"50000000"}},{}));
        std::cerr<< storage_service->getPropertyValueAsDouble(wrench::SimpleStorageServiceProperty::BUFFER_SIZE);
        storage_services.insert(storage_service);
    }

    std::string wms_host = hostname_list[0];

    // Instantiate a WMS
    auto wms = simulation.add(
            new SimpleWMS(storage_services, wms_host));
    wms->addWorkflow(workflow);

    std::cerr << "Launching the Simulation..." << std::endl;
    try {
        simulation.launch();
    } catch (std::runtime_error &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 0;
    }
    std::cerr << "Simulation done!" << std::endl;

    simulation.getOutput().dumpUnifiedJSON(workflow, "/tmp/simulation.json", false, false, false, false, false, true, false);

    return 0;
}

