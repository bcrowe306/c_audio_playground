from abc import ABC, abstractmethod


class Node(ABC):
    def __init__(self, value = None):
        super().__init__()
        self.__output_node: 'Node' = None
        self.__input_nodes: list['Node'] = []
        self.__value = value
        self.__on_input_received = None

    def __call__(self):
        return self.__value

    def get_value(self):
        return self.__value

    def get_output_node(self):
        return self.__output_node

    def set_value(self, value):
        self.__value = value
        for input_node in self.__input_nodes:
            if input_node.__on_input_received is not None:
                input_node.__on_input_received(self.__value)

    def send_to(self, input_node: 'Node'):
        """
        Connects the output of this node to the input of another node.
        """
        if input_node not in self.__input_nodes:
            self.__input_nodes.append(input_node)

        if input_node.get_output_node() != self:
            input_node.receive_from(self)
            

    def receive_from(self, output_node: 'Node'):
        """
        Connects the input of this node to the output of another node.
        """
        if output_node != self.__output_node:
            self.__output_node = output_node
        output_node.send_to(self)

    def update_from_input(self):
        if self.__output_node is not None:
            self.set_value(self.__output_node.get_value())

    def set_callback(self, on_input_received):
        self.__on_input_received = on_input_received


"""
Oneshot mode

note on: triggers the sample at the start of the sample
note off: does nothing
Polyphony = 1



"""