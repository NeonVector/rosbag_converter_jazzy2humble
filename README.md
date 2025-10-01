## Description

This program is designed to convert the *rosbag2* *jazzy* *"metadata.yaml"* style into a format that *humble* understands.

## jazzy and humble rosbag2 incompatibility issue

Any rosbags (*sqlite3* or *mcap* - it doesn't matter) written to *ROS2**jazzy* are not readable in *ROS2**humble*. There is a backward compatibility.

Ready-made solutions to this problem were not found, the problem itself was encountered on the Internet 1-2 times, the solution is not specified. Converting to other formats using standard rosbag2 tools didn't help.

Error itself:
```bash
Exception on parsing info file: yaml-cpp: error at line ... bad conversion
```

## The reason for the incompatibility of rosbags

By the method of research and comparison, it was determined that the reason for the unreadability of *jazzy* rosbags in *humble* lies in the fact that *jazzy* and *humble* *"metadata.yaml"* have different styles.

## Differences in the "metadata.yaml" style

1) The line `offered_qos_profiles:`

- **jazzy:**
```yaml jazzy
        offered_qos_profiles:
          - history: unknown
            depth: 10
            reliability: reliable
            durability: volatile
            deadline:
              sec: 9223372036
              nsec: 854775807
            lifespan:
              sec: 9223372036
              nsec: 854775807
            liveliness: automatic
            liveliness_lease_duration:
              sec: 9223372036
              nsec: 854775807
            avoid_ros_namespace_conventions: false
```
- **humble:**
```yaml humble
        offered_qos_profiles: "- history: 3\n  depth: 0\n  reliability: 1\n  durability: 2\n  deadline:\n    sec: 9223372036\n    nsec: 854775807\n  lifespan:\n    sec: 9223372036\n    nsec: 854775807\n  liveliness: 1\n  liveliness_lease_duration:\n    sec: 9223372036\n    nsec: 854775807\n  avoid_ros_namespace_conventions: false"
```

2) The lines `custom_data:`, `ros_distro:` at the end of the *humble* file do not exist.

- **jazzy:**
```yaml jazzy
  custom_data: ~
  ros_distro: jazzy
```

3) The line `type_description_hash:` in *humble* file do not exist.

- **jazzy:**
```yaml jazzy
    - topic_metadata:
        ...
        type_description_hash: RIHS01_e369d0f05a23ae52508854b66f6aa0437f3449d652e8cbf22d5abe85d020f087
      message_count: 12408
```


4) For some reason, the `offered_qos_profiles` information is sometimes duplicated in the *jazzy* file:

- **jazzy:**
```yaml jazzy
    - topic_metadata:
        name: /center
        type: sensor_msgs/msg/PointCloud
        serialization_format: cdr
        offered_qos_profiles:
          - history: unknown
            ...
            avoid_ros_namespace_conventions: false
          - history: unknown
            ...
            avoid_ros_namespace_conventions: false
        type_description_hash: RIHS01_614593df71d3c2b9bd4604a71b750fd218f0d65c045ea988b713719455a65b3b
      message_count: 15
```

## Purpose of the program

The program rewrites *jazzy* *"metadata.yaml"* into *humble* *"metadata.yaml"*, it simply refactors it.

It is based on a dictionary, written based on [ROS2 QoS policies](https://docs.ros.org/en/humble/Concepts/Intermediate/About-Quality-of-Service-Settings.html).

Program:
- changes the `offered_qos_profiles:` string (in *humble* style)
- deletes the `type_description_hash:` string (*humble* file doesn't have one)
- deletes the `custom_data:` string (*humble* file doesn't have one)
- deletes `ros_distro:` string (*humble* file doesn't have one)
- removes duplicate information of `offered_qos_profiles:` (*humble* file doesn't have it)

The program does not rewrite the source *"metadata.yaml"* file, but creates a new one based on it in the same directory.

## Program launch

The program consists of a single file that needs to be run in the *jazzy* *"metadata.yaml"* directory.

```bash
cd <your_rosbag_metadata_dir>
```
```bash
g++ -o rosbag2_metadata_jazzy_to_humble.out rosbag2_metadata_jazzy_to_humble.cpp
```
```bash
./rosbag2_metadata_jazzy_to_humble.out
```
## Program Parameters

Inside the code, you can specify the name of the output yaml file, by default it is *"metadata_humble.yaml"*.

## Program quality

The program was successfully tested on several rosbacks, and everything was displayed properly in rviz. *humble* run .db3 and .mcap rosbags of *jazzy*.

> However, rosbags converted by this program are not read by *jazzy* :)

If an error like this occurs when launching a converted rosbag:

```bash
requesting incompatible QoS. No messages will be sent to it. Last incompatible policy: RELIABILITY_QOS_POLICY
```
this means that the dictionary inside the program is formed incorrectly. Try changing the order of the numbers in the std::map dictionaries. It's possible that the keywords and meanings don't match correctly.

In an amicable way, it's needed to determine an unambiguous match in std::map based on some kind of ROS2 documentation, and not by experimenting. But for now, as it is.

I hope this information will be useful to someone. I will be glad to receive a feedback :)
