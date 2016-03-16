This program was inspired by the exercise in chapter 19 of "The Linux Programming Interface".
The program monitors a given directory and all its sub directories. The program will add new
directories to its watch list and remove deleted directories.

TODO
======
Finish implementing the read loop



Possible future version
==========================
re-design the list functions to encapsulate not only the management of the path key list
but to also add the directories to the inotify queue. For example we want to run the function
add_watch(fd, path). this function should add the path to the inotify queue associated with fd
and add add a pathkey to the list.

Enable configuration of the events to watch for via a configuration. The user can add a list of events to the config file and upon startup the eventmask is updated to reflect that configuration.