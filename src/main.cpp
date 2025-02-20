#include <iostream>
#include <fstream>
#include <filesystem>

#include "RenderWindow.h"
#include "TriangleMesh.h"
#include "RenderParameters.h"
#include "RenderController.h"

bool isHalfedgeFile(const std::string& rawMeshPath);

bool isTriFile(const std::string& rawMeshPath);

std::string extractMeshName(const std::string& rawMeshPath);

int main(int argc, char** argv) {
    QApplication application(argc, argv);

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <mesh file>" << std::endl;
        return 0;
    }

    TriangleMesh mesh;

    std::ifstream meshFile(argv[1]);

    // File is assumed to be .halfedge or .tri
    if (!meshFile.good() ||
        (isHalfedgeFile(argv[1]) && !mesh.readHalfedgeFile(meshFile)) ||
        (isTriFile(argv[1]) && !mesh.readTriFile(meshFile))) {
        std::cout << "Read failed for object " << argv[1] << std::endl;
        return 0;
    }

    RenderParameters renderParameters;

    RenderWindow renderWindow(&mesh, &renderParameters, argv[1]);
    RenderController renderController(&renderParameters, &renderWindow, extractMeshName(argv[1]));
    renderWindow.resize(1200, 675);
    renderWindow.show();

    return application.exec();
}

bool isHalfedgeFile(const std::string& rawMeshPath) {
    return std::filesystem::path(rawMeshPath).extension() == ".halfedge";
}

bool isTriFile(const std::string& rawMeshPath) {
    return std::filesystem::path(rawMeshPath).extension() == ".tri";
}

std::string extractMeshName(const std::string& rawMeshPath) {
    return std::filesystem::path(rawMeshPath).stem();
}
