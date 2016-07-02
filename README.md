# Thanos
## What is ?

Thanos is a tool made for scan a range of IP's and get the banners of the running services.
The main function is do banner grabber, but you also can use this for exploit vulnerabilities,
find proxy servers, or simply check for open ports.

### Compile:

You need sqlite3 and pthreads.

```sh
$ git clone https://github.com/P0cL4bs/Thanos.git
$ cd Thanos
$ make
```

### Using Thanos
 
Before starting scan, you must create a new database file, you can do it following the example: 
 
 ```
$ ./thanos --new-db banner_grabber.db
 
_____  _      __    _      ___   __  
 | |  | |_|  / /\  | |\ | / / \ ( (` 
 |_|  |_| | /_/--\ |_| \| \_\_/ _)_) 
 
        Banner Grabber v1.0.0 beta
 
 
[+] Creating new database file: banner_grabber.db
[+] Ok
[+] Creating table
[+] Executing query:
 
create table thanos_scan (
        id INTEGER PRIMARY KEY,
        port INTEGER NOT NULL,
        data TEXT,
        timeout INTEGER,
        limit_download INTEGER
);
 
[+] Ok
```
 
A new database file will be create, now you must insert new scanners configurations on
database:

```
$ echo -e "QUIT\n" > /tmp/ftp_scan
$ ./thanos --cfg-db banner_grabber.db \
 --cfg-port 21 \
 --cfg-file /tmp/ftp_scan \
 --cfg-timeout 10
 --cfg-limit 300
 
[...]
 
[+] Inserting new options to database file
[+] Ok
```
 
A new type of scan was insert on database, will scan the port 21, if it is open, will
send the data of file /tmp/ftp_scan for the port, and wait for 10 seconds, or until
the cfg-limit be reached, or when the server close the connection.
 
We can list the scanners in database using the option --list:

```
$ ./thanos --cfg-db banner_grabber.db --list
 
[...]
 
[+] Starting listing rows in database file
 
[+] ID: 1
[+] Port: 21
[+] Data: 515549540a0a
[+] Timeout: 10
[+] Limit Download: 300
 
[+] Finish
```
 
ID = Scanner ID

Port = Port to check

Data = Content to send

Timeout = Max time of seconds to wait

Limit Download = Max size of bytes to reached
 
if you want insert a new scanner, the option --cfg-port is required,
if --cfg-file not be set so the value will be NULL, if --cfg-timeout not be set
the value used will be the macro DEFAULT_TIMEOUT, if --cfg-limit not be set
the value used will be the macro DEFAULT_LIMIT_BYTES.

Doing the Scan:
 
```
$ ./thanos --cfg-db banner_grabber.db -s 0 -e 0
 
[...]
 
Number of scans load: 1
Total IP's to scan: 1
 
[+] 0.0.0.0:21 open
 
CHEWB FTP SERVER
 
```

-s | --start = IP address to start

-e | --end   = IP address to end
 
IP address samples:
 
1 = 1.0.0.0

1.2 = 1.2.0.0

1.2.3 = 1.2.3.0
 
you can configure the banner output, using -n (--no-wide) or -D (--hex-dump).
 
 
### Changing the configuration file:
 
You can edit the parameters of the configuration file , setting the option --update-id, example:

```
$ ./thanos --cfg-db banner_grabber.db --update-id 1 --cfg-data /dev/null
```

Change the Data value.
 
You can also delete scanners configurations, using --delete-id, example:

``` 
$ ./thanos --cfg-db banner_grabber.db --delete-id 1
```
