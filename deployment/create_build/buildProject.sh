#!/bin/bash

set -e

# Qt 5.15.17 configuration
QT_BASE_PATH="/usr/QT/5.15.17"

# Export Qt environment variables
export PATH="${QT_BASE_PATH}/bin:$PATH"
export LD_LIBRARY_PATH="${QT_BASE_PATH}/lib:$LD_LIBRARY_PATH"
export QT_PLUGIN_PATH="${QT_BASE_PATH}/plugins"
export CMAKE_PREFIX_PATH="${QT_BASE_PATH}/lib/cmake:$CMAKE_PREFIX_PATH"
export Qt5_DIR="${QT_BASE_PATH}/lib/cmake/Qt5"

# Verify Qt version
echo "Using Qt version:"
qmake -v

BUILD_DIR="/home/amin/Desktop/novin-repo/Novin-Repository-0_5/conversation_to_text/backend_cpp/CTT/deployment/build"
CREATE_BUILD_DIR="/home/amin/Desktop/novin-repo/Novin-Repository-0_5/conversation_to_text/backend_cpp/CTT/deployment/create_build"
DEBUG_BUILD_EXECUTABLE="../../etc/debug_build/CTT"
REMOTE_HOST="softnou@192.168.20.67"
REMOTE_PASS="softnou"
REMOTE_PATH="/home/softnou/SoftnouSmartAssistantBackend_Test"

echo "Cleaning local build directory..."
cd "$BUILD_DIR"
rm -rf ./*

echo "Setting AppImage permissions..."
cd "$CREATE_BUILD_DIR"
chmod +x ./linuxdeploy-x86_64.AppImage
chmod +x ./linuxdeploy-plugin-qt-x86_64.AppImage

echo "Running linuxdeploy with Qt plugin..."
# Pass Qt environment to linuxdeploy
QMAKE="${QT_BASE_PATH}/bin/qmake" \
./linuxdeploy-x86_64.AppImage --appdir="$BUILD_DIR" --executable="$DEBUG_BUILD_EXECUTABLE" --plugin=qt

echo "Creating usr.zip..."
cd "$BUILD_DIR"
zip -r usr.zip usr/

echo "Cleaning remote usr/ contents and old zip..."
sshpass -p "$REMOTE_PASS" ssh -o StrictHostKeyChecking=no "$REMOTE_HOST" \
"rm -rf $REMOTE_PATH/usr/bin/ $REMOTE_PATH/usr/lib/ $REMOTE_PATH/usr/plugins/ $REMOTE_PATH/usr/share/ $REMOTE_PATH/usr/translations/ && rm -f $REMOTE_PATH/usr.zip"

echo "Uploading usr.zip to remote..."
sshpass -p "$REMOTE_PASS" scp -o StrictHostKeyChecking=no usr.zip "$REMOTE_HOST:$REMOTE_PATH"

echo "Unzipping usr.zip on remote..."
sshpass -p "$REMOTE_PASS" ssh -tt -o StrictHostKeyChecking=no "$REMOTE_HOST" \
"cd $REMOTE_PATH && unzip -q usr.zip"

echo "Post-build deployment completed successfully."

