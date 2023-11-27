class LabelAlreadyDefinedException(Exception):
    def __init__(self, message) -> None:
        super().__init__(f'Label {message} must be defined only once')

class InvalidInstructionException(Exception):
    def __init__(self, message) -> None:
        super().__init__(f'Operation {message} cannot be parsed')