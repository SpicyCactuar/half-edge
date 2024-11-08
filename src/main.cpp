#include <iostream>
#include <fstream>
#include <filesystem>

#include "RenderWindow.h"
#include "TriangleMesh.h"
#include "RenderParameters.h"
#include "RenderController.h"

bool isHdsFile(const std::string& rawMeshPath);

bool isTriFile(const std::string& rawMeshPath);

std::string extractMeshPathPrefix(const std::string& rawMeshPath);

int main(int argc, char** argv) {
    QApplication application(argc, argv);

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <mesh file>" << std::endl;
        return 0;
    }

    TriangleMesh mesh;

    std::ifstream meshFile(argv[1]);

    // File is assumed to be .hds or .tri
    if (!meshFile.good() ||
        (isHdsFile(argv[1]) && !mesh.readHdsFile(meshFile)) ||
        (isTriFile(argv[1]) && !mesh.readTriFile(meshFile))) {
        std::cout << "Read failed for object " << argv[1] << std::endl;
        return 0;
    }

    RenderParameters renderParameters;

    RenderWindow renderWindow(&mesh, &renderParameters, argv[1], extractMeshPathPrefix(argv[1]));
    RenderController renderController(&mesh, &renderParameters, &renderWindow);
    renderWindow.resize(1200, 675);
    renderWindow.show();

    return application.exec();
}

bool isHdsFile(const std::string& rawMeshPath) {
    return std::filesystem::path(rawMeshPath).extension() == ".hds";
}

bool isTriFile(const std::string& rawMeshPath) {
    return std::filesystem::path(rawMeshPath).extension() == ".tri";
}

std::string extractMeshPathPrefix(const std::string& rawMeshPath) {
    const std::filesystem::path meshPath(rawMeshPath);
    const auto outFolder = meshPath.root_path() / "out";
    if (!exists(outFolder)) {
        create_directories(outFolder);
    }
    return outFolder / meshPath.stem();
}
