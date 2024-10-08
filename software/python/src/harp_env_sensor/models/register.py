import logging

from dataclasses import dataclass, field, fields
from pyharp.messages import HarpMessage, ReplyHarpMessage
from typing import List, Union


@dataclass
class Register:
    """
    Register class represents a single register in a pyharp device.
    The yml file that defines the register is in the following directory: StageWidget/configs/device.yml
    """
    name: str
    address: int
    type: str
    access: str
    description: str
    defaultValue: int = field(default=10)
    maxValue: int = field(default=10)
    minValue: int = field(default=10)
    reply: ReplyHarpMessage = field(default=None)

    def __init__(self, name, **kwargs):
        self.name = name

        # Get all the attributes in the Register class
        attributes = set([f.name for f in fields(self)])

        # For each of our register, populate the attributes with values
        for k, v in kwargs.items():
            if k in attributes:
                setattr(self, k, v)

        if not hasattr(HarpMessage, f"Read{self.type}"):
            logging.error(f"Message payload type not supported: {self.type}")
            raise Exception(f"Message payload type not supported: {self.type}")

        self.read_message = getattr(HarpMessage, f"Read{self.type}")
        self.write_message = getattr(HarpMessage, f"Write{self.type}")

    def create_read_message(self) -> ReplyHarpMessage:
        return self.read_message(self.address)

    def create_write_message(self, value: Union[int, List[int]]) -> ReplyHarpMessage:
        return self.write_message(self.address, value)

