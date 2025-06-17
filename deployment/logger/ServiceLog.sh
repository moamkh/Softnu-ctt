#!/bin/bash

# Replace YOUR_SERVICE_NAME with the actual name of your service
SERVICE_NAME="s_aichatbot.service"

echo "Troubleshooting logs for $SERVICE_NAME"

# Check if the service is managed by systemd
if systemctl list-unit-files | grep -q $SERVICE_NAME; then
    echo "Service is managed by systemd"
    echo "Checking logs with journalctl:"
    sudo journalctl -u $SERVICE_NAME -n 50
else
    echo "Service not found in systemd"
fi

# Check common log locations
log_locations=(
    "/var/log/$SERVICE_NAME.log"
    "/var/log/$SERVICE_NAME/$SERVICE_NAME.log"
    "/opt/$SERVICE_NAME/logs/$SERVICE_NAME.log"
)

for log in "${log_locations[@]}"; do
    if [ -f "$log" ]; then
        echo "Found log file: $log"
        echo "Last 50 lines of the log:"
        tail -n 50 "$log"
    fi
done

# Check the service configuration for custom log paths
if [ -f "/etc/systemd/system/$SERVICE_NAME.service" ]; then
    echo "Checking service configuration for custom log paths:"
    grep -i "standard\(output\|error\)" "/etc/systemd/system/$SERVICE_NAME.service"
fi

# If we haven't found logs yet, try to find the process and check its file descriptors
PID=$(pgrep -f "$SERVICE_NAME")
if [ -n "$PID" ]; then
    echo "Found process with PID: $PID"
    echo "Checking file descriptors:"
    ls -l /proc/$PID/fd
    echo "You might be able to view stdout with: tail -f /proc/$PID/fd/1"
    echo "You might be able to view stderr with: tail -f /proc/$PID/fd/2"
else
    echo "Could not find a running process for $SERVICE_NAME"
fi

echo "Troubleshooting complete. If you couldn't find your logs, you may need to modify your service configuration to redirect output to a file."
