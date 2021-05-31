vcpkg_installed_dir=$1

jsoncpp=${vcpkg_installed_dir}/share/jsoncpp
lyra2=${vcpkg_installed_dir}/share/lyra2-file-encryptor

cmake -G "Ninja" -B build -DCMAKE_BUILD_TYPE=Release -DWEBUI=OFF -Djsoncpp_DIR=${jsoncpp} -Dlyra2-file-encryptor_DIR=${lyra2} -DQt5LinguistTools_DIR=/usr/lib/x86_64-linux-gnu/cmake/Qt5LinguistTools -D_VCPKG_INSTALLED_DIR=${vcpkg_installed_dir}