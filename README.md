# Password-Network

A distributed network that hashes a range of potenial passwords using MD5 until a match is found.

## Authors

* **Myles Becker** - [beckermy](https://github.com/beckermy)
* **Seth Ruiz** - [SETHRUIZ](https://github.com/SETHRUIZ)
* **Tapiwa Zvidzwa** - [Tapszvidzwa](https://github.com/Tapszvidzwa)

## Execution

* Run 'make' to ensure the program is compiled
* Open a terminal and navigate to the directory containing the password network and run './password-server'
* Write in the md5 hash that you want to decrypt when prompted, then hit enter
* For example: ac940682a28f8a3798f1232afb3872d1
* Open several other terminals and navigate to the directory mentioned previously, preferably on a number of different machines
* Run './password-client' followed by either 'localhost' if on the same machine as the terminal running './password-server' or follow with the address of that machine, for example 'burroughs.cs.grinnell.edu'
* The server should indicate as each client connects that a connection has been established and gives it a client id
* The clients should indicate that they are searching 
* If one of the clients finds the password, it will tell the server and all other clients will disconnect
* The password will appear on the terminal running the server as well as the terminal running the client that found the password
* For example, the provided hash will be 'zvidzwa'

## Contributions

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Thank you [PurpleBooth](https://github.com/PurpleBooth) for the contributions template.
