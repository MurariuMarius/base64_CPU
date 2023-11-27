class LabelAlreadyDefinedException(Exception):
    def __init__(self, message, line) -> None:
        super().__init__(f'Label {message} at line {line} has already been defined')