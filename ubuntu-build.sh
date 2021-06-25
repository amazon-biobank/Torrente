vcpkg_installed_dir=$1

jsoncpp=${vcpkg_installed_dir}/share/jsoncpp
lyra2=${vcpkg_installed_dir}/share/lyra2-file-encryptor

cmake -G "Ninja" -B build -DCMAKE_BUILD_TYPE=Release -DWEBUI=OFF -Djsoncpp_DIR=${jsoncpp} -Dlyra2-file-encryptor_DIR=${lyra2} -D_VCPKG_INSTALLED_DIR=${vcpkg_installed_dir}
