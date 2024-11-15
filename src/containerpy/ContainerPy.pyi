from typing import List, Dict
from enum import Enum

class Package:
    """
    Represents a package that can be stored in a container.

    Methods:
        - __init__: Initializes a package with an ID or from a JSON-like dictionary.
        - get_package_id: Retrieves the ID of the package.
        - set_package_id: Sets a new ID for the package.
        - to_json: Extracts package information into a Python dictionary.
    """

    def __init__(self, id: str) -> None:
        """
        Initializes a Package instance with the specified ID.

        Args:
            id (str): The unique identifier for the package.
        """
        ...

    def __init__(self, json_dict: Dict) -> None:
        """
        Initializes a Package instance from a JSON-like dictionary.

        Args:
            json_dict (Dict): A dictionary containing package details.
        """
        ...

    def get_package_id(self) -> str:
        """
        Retrieves the package ID.

        Returns:
            str: The ID of the package.
        """
        ...

    def set_package_id(self, id: str) -> None:
        """
        Sets the ID of the package.

        Args:
            id (str): The new ID to set for the package.
        """
        ...

    def to_json(self) -> Dict:
        """
        Converts the package details into a JSON-like dictionary.

        Returns:
            Dict: A dictionary containing the package information.
        """
        ...

class Container:
    """
    Represents a container that can hold packages and track its details.

    Methods:
        - __init__: Initializes a container with an ID, size, or from a JSON-like dictionary.
        - get_container_id: Retrieves the ID of the container.
        - set_container_id: Sets a new ID for the container.
        - get_container_size: Retrieves the size of the container.
        - set_container_size: Sets a new size for the container.
        - add_package: Adds a package to the container.
        - add_custom_variable: Adds custom variables to the container.
        - remove_custom_variable: Removes a specific custom variable.
        - get_custom_variable: Retrieves a custom variable's value.
        - set_container_current_location: Sets the current location of the container.
        - get_container_current_location: Retrieves the current location of the container.
        - set_container_next_destinations: Sets the next destinations for the container.
        - get_container_next_destinations: Retrieves the list of next destinations.
        - set_container_movement_history: Sets the movement history for the container.
        - get_container_movement_history: Retrieves the movement history.
        - to_json: Extracts container information into a Python dictionary.
    """

    def __init__(self, id: str, size: int) -> None:
        """
        Initializes a Container instance with the specified ID and size.

        Args:
            id (str): The unique identifier for the container.
            size (int): The size of the container.
        """
        ...
    def __init__(self, json_dict: Dict) -> None:
        """
        Initializes a Container instance from a JSON-like dictionary. Mandatory fields include:
            - containerID (str): Unique identifier for the container.
            - containerSize (float): Size of the container.
            - addedTime (float): Timestamp indicating when the container was added.

        Args:
            json_dict (Dict): A dictionary containing container details.
        """
        ...

    def get_container_id(self) -> str:
        """
        Retrieves the container ID.

        Returns:
            str: The ID of the container.
        """
        ...

    def set_container_id(self, id: str) -> None:
        """
        Sets the ID of the container.

        Args:
            id (str): The new ID to set for the container.
        """
        ...
    def get_container_size(self) -> int:
        """
        Retrieves the size of the container.

        Returns:
            int: The size of the container.
        """
        ...

    def set_container_size(self, size: int) -> None:
        """
        Sets the size of the container.

        Args:
            size (int): The new size to set for the container.
        """
        ...

    def add_package(self, package: Package) -> None:
        """
        Adds a package to the container.

        Args:
            package (Package): The package to add.
        """
        ...

    def get_packages(self) -> List[Package]:
        """
        Gets packages list from the container.

        Returns:
            List[packages]: The packages list in the container.
        """
        ...

    def get_container_added_time(self):
        """
        Gets the container added time.
        
        Returns:
            float: the time.
        """

    def set_container_added_time(self, time:float):
        """
        Sets the container added time.

        Args:
            time (float): the time.
        """
        ...

    def get_container_leaving_time(self):
        """
        Gets the container leaving time.
        
        Returns:
            float: the time.
        """

    def set_container_leaving_time(self, time:float):
        """
        Sets the container leaving time.

        Args:
            time (float): the time.
        """
        ...

    def add_custom_variable(self, hauler: int, key: str, value: str) -> None:
        """
        Adds a custom string variable to the container.

        Args:
            hauler (int): Represents the hauler type (e.g., truck, train).
            key (str): The key for the custom variable.
            value (str): The string value to store.
        """
        ...

    def add_custom_variable(self, hauler: int, key: str, value: int) -> None:
        """
        Adds a custom integer variable to the container.

        Args:
            hauler (int): Represents the hauler type (e.g., truck, train).
            key (str): The key for the custom variable.
            value (int): The integer value to store.
        """
        ...

    def add_custom_variable(self, hauler: int, key: str, value: float) -> None:
        """
        Adds a custom float variable to the container.

        Args:
            hauler (int): Represents the hauler type (e.g., truck, train).
            key (str): The key for the custom variable.
            value (float): The float value to store.
        """
        ...

    def remove_custom_variable(self, hauler: int, key: str) -> None:
        """
        Removes a custom variable from the container.

        Args:
            hauler (int): Represents the hauler type.
            key (str): The key of the custom variable to remove.
        """
        ...

    def get_custom_variable(self, hauler: int, key: str) -> str:
        """
        Retrieves the value of a custom variable as a string.

        Args:
            hauler (int): Represents the hauler type.
            key (str): The key of the custom variable.

        Returns:
            str: The value of the custom variable as a string.
        """
        ...

    def set_container_current_location(self, location: str) -> None:
        """
        Sets the current location of the container.

        Args:
            location (str): The new location.
        """
        ...

    def get_container_current_location(self) -> str:
        """
        Retrieves the current location of the container.

        Returns:
            str: The container's current location.
        """
        ...

    def set_container_next_destinations(self, destinations: List[str]) -> None:
        """
        Sets the next destinations for the container.

        Args:
            destinations (List[str]): A list of next destinations.
        """
        ...

    def get_container_next_destinations(self) -> List[str]:
        """
        Retrieves the list of next destinations for the container.

        Returns:
            List[str]: A list of next destinations.
        """
        ...

    def set_container_movement_history(self, history: List[str]) -> None:
        """
        Sets the movement history for the container.

        Args:
            history (List[str]): A list of locations the container has been through.
        """
        ...

    def get_container_movement_history(self) -> List[str]:
        """
        Retrieves the movement history of the container.

        Returns:
            List[str]: A list of locations the container has been through.
        """
        ...

    def to_json(self) -> Dict:
        """
        Converts the container details into a JSON-like dictionary.

        Returns:
            Dict: A dictionary containing the container information.
        """
        ...


class ContainerMap:
    """
    Represents a map of containers, used to manage and retrieve containers.

    Methods:
        - __init__: Initializes a ContainerMap instance.
        - add_container: Adds a container to the map.
        - add_containers: Adds multiple containers.
        - add_containers_from_dict: Adds containers using a JSON-like dictionary.
        - get_container: Retrieves a container by its ID.
        - remove_container: Removes a container by ID.
        - get_all_containers: Retrieves all containers in the map (use with caution).
        - get_latest_containers: Retrieves the most recently added containers.
        - size: Retrieves the number of containers in the map.
        - get_containers_by_added_time: Retrieves containers based on time criteria.
        - dequeue_containers_by_added_time: Removes containers based on time criteria.
        - get_containers_by_next_destination: Retrieves containers heading to a specific destination.
        - dequeue_containers_by_next_destination: Removes containers heading to a specific destination.
        - to_json: Extracts ContainerMap information into a Python dictionary.
    """

    def __init__(self) -> None:
        """
        Initializes an empty ContainerMap.
        """
        ...

    def __init__(self, dbLocation: str) -> None:
        """
        Initializes a ContainerMap connected to a database.

        Args:
            dbLocation (str): The file path to the database.
        """
        ...

    def __init__(self, json_dict: Dict) -> None:
        """
        Initializes a ContainerMap from a JSON-like dictionary.

        Args:
            json_dict (Dict): A dictionary containing map details.
        """
        ...

    def add_container(self, container: Container, addingTime: float = float('nan'), leavingTime: float = float('nan')) -> None:
        """
        Adds a container to the map.

        Args:
            container (Container): The container to add.
            addingTime (float, optional): The time when the container was added. Defaults to NaN.
            leavingTime (float, optional): The time when the container should leave the host. Defaults to NaN.
        """
        ...

    def add_containers(self, containers: List[Container], addingTime: float = float('nan'), leavingTime: float = float('nan')) -> None:
        """
        Adds multiple containers to the map.

        Args:
            containers (List[Container]): A list of containers to add.
            addingTime (float, optional): The time when the containers were added. Defaults to NaN.
            leavingTime (float, optional): The time when the container should leave the host. Defaults to NaN.

        """
        ...

    def add_containers_from_dict(self, json_dict: Dict, addingTime: float = float('nan'), leavingTime: float = float('nan')) -> None:
        """
        Adds multiple containers using a JSON-like dictionary.

        Args:
            json_dict (Dict): A dictionary containing container details.
            addingTime (float, optional): The time when the containers were added. Defaults to NaN.
            leavingTime (float, optional): The time when the container should leave the host. Defaults to NaN.
        """
        ...

    def get_container_by_id(self, id: str) -> Container:
        """
        Retrieves a container by its ID.

        Args:
            id (str): The ID of the container.

        Returns:
            Container: The container associated with the ID.
        """
        ...

    def remove_container_by_id(self, id: str) -> None:
        """
        Removes a container by its ID.

        Args:
            id (str): The ID of the container to remove.
        """
        ...

    def get_all_containers(self) -> List[Container]:
        """
        Retrieves all containers from the map. Use with caution, especially with large datasets.

        Returns:
            List[Container]: A list of all containers.
        """
        ...

    def get_latest_containers(self) -> List[Container]:
        """
        Retrieves up to the last 200 containers added to the map.

        Returns:
            List[Container]: A list of the latest containers.
        """
        ...

    def size(self) -> int:
        """
        Retrieves the number of containers in the map.

        Returns:
            int: The number of containers in the map.
        """
        ...

    def get_containers_by_added_time(self, referenceTime: float, condition: str) -> List[Container]:
        """
        Retrieves containers based on an added time condition.

        Args:
            referenceTime (float): The time to compare.
            condition (str): The condition to apply ("before" or "after").

        Returns:
            List[Container]: A list of containers matching the criteria.
        """
        ...

    def dequeue_containers_by_added_time(self, referenceTime: float, condition: str) -> List[Container]:
        """
        Removes containers based on an added time condition.

        Args:
            referenceTime (float): The time to compare.
            condition (str): The condition to apply ("before" or "after").

        Returns:
            List[Container]: A list of removed containers.
        """
        ...

    def get_containers_by_leaving_time(self, referenceTime: float, condition: str) -> List[Container]:
        """
        Retrieves containers based on a leaving condition.

        Args:
            referenceTime (float): The time to compare.
            condition (str): The condition to apply ("before" or "after").

        Returns:
            List[Container]: A list of containers matching the criteria.
        """
        ...

    def dequeue_containers_by_leaving_time(self, referenceTime: float, condition: str) -> List[Container]:
        """
        Removes containers based on a leaving time condition.

        Args:
            referenceTime (float): The time to compare.
            condition (str): The condition to apply ("before" or "after").

        Returns:
            List[Container]: A list of removed containers.
        """
        ...

    def get_containers_by_next_destination(self, destination: str) -> List[Container]:
        """
        Retrieves containers heading to a specified destination.

        Args:
            destination (str): The next destination to search for.

        Returns:
            List[Container]: A list of containers heading to the destination.
        """
        ...

    def dequeue_containers_by_next_destination(self, destination: str) -> List[Container]:
        """
        Removes containers heading to a specified destination.

        Args:
            destination (str): The destination to remove containers for.

        Returns:
            List[Container]: A list of removed containers.
        """
        ...

    def to_json(self) -> Dict:
        """
        Converts the ContainerMap details into a JSON-like dictionary.

        Returns:
            Dict: A dictionary containing the ContainerMap information.
        """
        ...
        
class ContainerSize(Enum):
    """
    Enumeration representing various container sizes.

    Attributes:
        TwentyFT: Standard 20-foot container.
        TwentyFT_HighCube: 20-foot high-cube container.
        FourtyFT: Standard 40-foot container.
        FourtyFT_HighCube: 40-foot high-cube container.
        FortyFiveFT: Standard 45-foot container.
        FortyFiveFT_HighCube: 45-foot high-cube container.
        TenFT: Standard 10-foot container.
        ThirtyFT: Standard 30-foot container.
        FortyEightFT: Standard 48-foot container.
        FiftyThreeFT: Standard 53-foot container.
        SixtyFT: Standard 60-foot container.
    """
    TwentyFT = ...
    TwentyFT_HighCube = ...
    FourtyFT = ...
    FourtyFT_HighCube = ...
    FortyFiveFT = ...
    FortyFiveFT_HighCube = ...
    TenFT = ...
    ThirtyFT = ...
    FortyEightFT = ...
    FiftyThreeFT = ...
    SixtyFT = ...

class HaulerType(Enum):
    """
    Enumeration representing different types of haulers.

    Attributes:
        Truck: Road transport vehicle.
        Train: Rail transport vehicle.
        WaterTransport: Water-based transport vessel.
        AirTransport: Aircraft used for transport.
    """
    Truck = ...
    Train = ...
    WaterTransport = ...
    AirTransport = ...
