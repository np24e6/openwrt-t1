# Cumulocity 

Cloud stream service which can be connected to `www.softwareag.cloud`, `https://telekom.com` platforms for basic device monitoring and controlling utilities. Both platforms are identical, the main difference is that `www.softwareag.cloud` can be used with unsecure connection (contact service providers for additional information about unsecured connection). 

Service collects `device`, `firmware`, `hardware` and `mobile` data from router to build a template on selected platform. After successfull connection it streams RSSI signal by given time interval.

## Dependencies

Required dependencies: `libsera`, `libuci`, `libgsm`, `libmnfinfo`, `libpthread`.

Service is written in cpp in order to use libsera (source: https://bitbucket.org/m2m/cumulocity-sdk-c.git), which does the main work for making connections and streaming data.

## Configuration

Each platform represents section in `/etc/config/iot` file. 

`www.softwareag.cloud` aka `cumulocity` 
`https://telekom.com` aka `cloudofthings`

Option name | Type | Optional | Possible variables | Default | Description
--- | --- | --- | --- | --- | ---
enabled | int | false | 0 or 1 | 0 | option to identify if service is enabled
ssl | string | false (true for cumulocity) | 0 - 1 | 0 - 1 | Establishes secure connection
iface | string | false |  | mob1s1a1 | mobile network interface for gsm value collection
server | string | false | hostname | (Empty) | describes server address for communication
tenant | string | false | - | - | used to establish connection
username | string | false | - | device + serial | used to establish connection
password | string | false | - | - | used to establish connection 

## Platform credentials

After registering device to one of the platforms, credential file will be created: `/tmp/cumulocity`, `/tmp/cloudofthings`. File stores tenant, username and password data. Each time when enabling service libsera function reads this file. To preserve credentials if file is lost, their data is read and stored to config.

In order to re-register device, credentials from config and credentials file has to be deleted.

