# H1 Project purpose
Project allows to get geolocation (latitiude and longtitiude) of any IP address or URL. If search is done through URL, then first it is converted to list of corresponding IP addresses. Then geolocation for each IP address is stored in DB.

If database already contains specific IP address or URL, then geolocation for this location is returned imidiately. Otherwise, the request for geolocation is performed to online location provider (ipstack.com)

# H1 Preparing environment
List of requirements:
 - CMake
 - SQLite database
 - QT 6.5
 - Google Test/Google Mocks (only for tests)

To setup environment for the first time run script setup.sh (for Debian based distributions). Script will install all requirements in the system. It will also clone google test repository, and install it. Script requires admin rights to run.

After installing required packages, you need to create database. In this database, will be stored geolocations of ip addresses/URLs. Application at the moment supports only SQLite3. You can create empty database (in your home directory) with command:
```
sqlite3 ~/geolocations.db
```

Beside setting up environment locally, you also need an API key for ipstack.com. To get it, create an account on this website. After logging in, on main website you should see your API key:
```
Your API Access Key:
```
Please remember, that there is limitation to 100 free requests per month.

# H1 Building an application
Build system uses cmake. To build a project just run build.sh script.

# H1 Running the application
After building the applicatoin you can simply start it from command line:
```
./geolocation
```
Application is stores in directory: out/app

After startup, you need to provide configuration that contains:
 1. API key to ipstack.com
 2. Database location (e.g. ~/geolocations.db)
 3. Table name (for storing geolocations)

Table stores geolocations will have following schema:
```
Latitiude, Longtitiude, Ip Address, URL
```

Please keep in mind, that URL will be filled up only if URL will be used to search location

# H1 Known limitations
Providing full url address may not work yet. Please provide URL without protocol (http(s)) and www prefix.

# H1 Contact
In case of any questions please write to mati_s3@o2.pl