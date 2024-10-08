import logging
import os
import serial.tools.list_ports as list_ports

from pyharp.device import Device
from pyharp.messages import MessageType, ReplyHarpMessage
from typing import Dict, List, Optional, Union

from ..models.register import Register


class EnvSensorDevice(Device):
    """
    StepperMotorDevice class represents the StepperMotor pyharp DEVICE.
    This device can control up to 4 separate stepper motors.
    """

    def __init__(self, com_port: Optional[str] = None, config: Dict = {}, dump_file: str = "ibl.bin"):
        """
        Initialize the StepperMotorDevice class

        Stepper Motor Device contains 4 encoder, 4 inputs, and 4 stepper motors connected to it.
        """
        self.config = config

        # Create directory for dump_file if it doesn't already exist
        dump_file_path = config["dump_file_path"]
        if not os.path.exists(dump_file_path):
            os.makedirs(dump_file_path)

        # Automatically read com ports if one is not given
        if not com_port:
            for port in list_ports.comports():
                if port.pid == 24577:  # All harp devices have the same PID
                    com_port = port.device
                    super().__init__(com_port, dump_file_path + dump_file)
                    if self.WHO_AM_I == 1234:  # Stepper Device ID
                        break
                    else:
                        self.disconnect()
        else:
            super().__init__(com_port, dump_file_path + dump_file)

        logging.info(f"Connected to {com_port}, device id: {self.WHO_AM_I}")

        # Add register as an attribute
        self.__dict__["registers"] = self.load_registers()

    def __getattr__(self, attr: str) -> List[int]:
        """
        Override default getattr. This was done to dynamically add all registers as attributes to this instance

        Note: this method only gets called when default attribute access fails. If an attribute is added in the init
        of this class, it will not run through this method when accessing it.

        :param attr: name of the attribute
        :return: value in the register
        """
        if "registers" in self.__dict__ and attr in self.__dict__["registers"]:
            message = self.__dict__["registers"][attr].create_read_message()
            reply = self.send(message.frame)  # send bytearray of the harp message
            if reply.message_type == MessageType.READ_ERROR:
                raise ValueError(f"Error reading from register: {attr}, " f"Reply Message: {reply}")
            return reply.payload

    def __setattr__(self, attr: str, value: Union[int, List[int]]) -> None:
        """
        Override default setattr. This was done to dynamically add all registers as attributes to this instance

        Note: this method only gets called when default attribute setter fails. If an attribute is added in the init
        of this class, it will not run through this method when setting it.

        :param attr: name of the attribute
        :param value: value to write to the register
        """
        if "registers" in self.__dict__ and attr in self.__dict__["registers"]:
            message = self.__dict__["registers"][attr].create_write_message(value)
            reply = self.send(message.frame)  # send bytearray of the harp message
            if reply.message_type == MessageType.WRITE_ERROR:
                raise ValueError(f"Error writing to register: {attr} with value: {value}, " f"Reply Message: {reply}")
        else:
            super().__setattr__(attr, value)

    def __dir__(self) -> List:
        """
        Override dir method. Adds registers to list that gets returned when running dir()

        :return: dir() + list of registers added as attributes
        """
        return dir(super()) + [r for r in self.registers]

    def load_registers(self) -> Dict[str, Register]:
        """
        Load registers defined in device.yml file (https://github.com/harp-tech/device.stepperdriver/blob/main/device.yml)

        :return: dictionary of register names as keys and register objects as values
        """
        logging.info("Getting available registers on device")

        # Read device_configuration
        device_configuration = self.config["device_configuration"]

        # Check firmware version of device matches the configuration
        firmware_version = f"{self.FIRMWARE_VERSION_H}.{self.FIRMWARE_VERSION_L}"
        if (config_version := device_configuration["firmwareVersion"]) != firmware_version:
            logging.warning(
                f"Firmware version {firmware_version} doesn't match config version {config_version}.\n"
                "    Registers may be defined incorrectly"
            )

        # Create and return register dictionary
        return {
            register_name: Register(register_name, **values)
            for register_name, values in device_configuration["registers"].items()
        }
