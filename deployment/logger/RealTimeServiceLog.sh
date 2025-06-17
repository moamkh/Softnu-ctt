#!/bin/sh

# Replace YOUR_SERVICE_NAME with the actual name of your service
SERVICE_NAME="s_aichatbot.service"

echo "Troubleshooting logs for $SERVICE_NAME"

# Function to view logs in real-time
view_realtime() {
    cmd="$1"
    echo "Viewing logs in real-time. Press Ctrl+C to exit."
    eval "$cmd"
}

# Check if the service is managed by systemd
if systemctl list-unit-files | grep -q "$SERVICE_NAME"; then
    echo "Service is managed by systemd"
    echo "To view logs in real-time, run:"
    echo "sudo journalctl -u $SERVICE_NAME -f"
    printf "View systemd logs in real-time? (y/n): "
    read choice
    if [ "$choice" = "y" ]; then
        view_realtime "sudo journalctl -u $SERVICE_NAME -f"
        exit 0
    fi
else
    echo "Service not found in systemd"
fi

# Check common log locations
check_log() {
    log="$1"
    if [ -f "$log" ]; then
        echo "Found log file: $log"
        echo "To view this log in real-time, run:"
        echo "tail -f $log"
        printf "View this log in real-time? (y/n): "
        read choice
        if [ "$choice" = "y" ]; then
            view_realtime "tail -f $log"
            exit 0
        fi
    fi
}

check_log "/var/log/$SERVICE_NAME.log"
check_log "/var/log/$SERVICE_NAME/$SERVICE_NAME.log"
check_log "/opt/$SERVICE_NAME/logs/$SERVICE_NAME.log"

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
    echo "To view stdout in real-time, run:"
    echo "tail -f /proc/$PID/fd/1"
    echo "To view stderr in real-time, run:"
    echo "tail -f /proc/$PID/fd/2"
    printf "View stdout in real-time? (y/n): "
    read choice
    if [ "$choice" = "y" ]; then
        view_realtime "tail -f /proc/$PID/fd/1"
        exit 0
    fi
    printf "View stderr in real-time? (y/n): "
    read choice
    if [ "$choice" = "y" ]; then
        view_realtime "tail -f /proc/$PID/fd/2"
        exit 0
    fi
else
    echo "Could not find a running process for $SERVICE_NAME"
fi

echo "Troubleshooting complete. If you couldn't find your logs, you may need to modify your service configuration to redirect output to a file."
