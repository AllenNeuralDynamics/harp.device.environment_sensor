from pathlib import Path
import yaml
import time

from pyharp.device import DeviceMode, Device

from harp_env_sensor import EnvSensorDevice

com_port = "COM4"

device_yaml = """
%YAML 1.1
---
# yaml-language-server: $schema=https://raw.githubusercontent.com/harp-tech/reflex-generator/main/schema/device.json
device: EnvironmentSensor
whoAmI: 1845
firmwareVersion: "0.1"
hardwareTargets: "0.1"
registers:
  Pressure:
    address: 32
    type: U32
    description: Pressure, in Pa
  Temperature:
    address: 33
    type: Float
    description: Temperature in degrees C
  Humidity:
    address: 34
    type: Float
    description: Humidity, in %RH
"""

config_dict = {
    "dump_file_path": str(Path(__file__).parent),
    "device_configuration": yaml.safe_load(device_yaml),
}

device = EnvSensorDevice(com_port, config_dict)
# device = Device(com_port)


device.set_mode(DeviceMode.Active)
while True:
    print(f"Pressure: {device.Pressure}, Temperature: {device.Temperature}, Humidity: {device.Humidity}")
    time.sleep(1.5)

    # event_response = device._read()  # read any incoming events.
    # if event_response is not None:  # and event_response.address != 44:
    #     print()
    #     print(event_response)


pass
