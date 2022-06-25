<h1 align="center">
	Client-Server chat
</h1>

<p align="center">
	Simple C++ Client-Server chat using sockets.
</p>

<p align="center">
	<img alt="GitHub code size in bytes" src="https://img.shields.io/github/languages/code-size/haimasker/Client-Server-chat?color=blue" />
	<img alt="Number of lines of code" src="https://img.shields.io/tokei/lines/github/haimasker/Client-Server-chat?color=blue" />
	<img alt="Code language count" src="https://img.shields.io/github/languages/count/haimasker/Client-Server-chat?color=blue" />
	<img alt="GitHub top language" src="https://img.shields.io/github/languages/top/haimasker/Client-Server-chat?color=blue" />
</p>

<h3 align="center">
	<a href="#preamble">Preamble</a>
	<span> · </span>
	<a href="#operations">Operations</a>
	<span> · </span>
	<a href="#examples">Examples</a>
</h3>

---

<a name="preamble"></a>
## Preamble

The purpose of this project is to create Client-Server chat with [winsock2](https://docs.microsoft.com/en-us/windows/win32/api/winsock2/). <br>
The concept of this programm is that clients can create chat rooms and communicate within them.

---

<a name="operations"></a>
## Operations

* Сlient and server have their own functions and features:

| Client                         | Server                       |
|:------------------------------:|:----------------------------:|
| Server connection              | Message forwarding           |
| Server disconnection           | Logging user actions         |
| Creating rooms                 | Showing warning messages     |
| Removing rooms                 | Showing information messages |
| Opening existing rooms         |                              |
| Exiting rooms                  |                              |
| Showing list of existing rooms |                              |
| Changing nickname              |                              |

<br>

* There are several types of messages in the client console, which are identified by their color:

| Color | Description                      |
|:-----:|:--------------------------------:|
| Green | User messages or commands        |
| White | Messages from other users        |
| Blue  | Information messages from server |
| Red   | Warning messages from server     |

<br>

* And several types of messages in the server console:

| Color | Description                                                                                         |
|:-----:|:---------------------------------------------------------------------------------------------------:|
| Green | Client connection message                                                                           |
| White | Forwarding messages between users                                                                   |
| Blue  | Client command information                                                                          |
| Red   | Receiving unrecognized packet from client <br> Client disconnection <br> Exceeding connection limit |

---

<a name="examples"></a>
## Examples

* Let's look at a variety of informational and warning messages from the client side. <br> Server on the left, client on the right. <br>

![1](https://github.com/Haimasker/Client-Server-chat/blob/master/images/1.png) <br>

![2](https://github.com/Haimasker/Client-Server-chat/blob/master/images/2.png) <br>

![3](https://github.com/Haimasker/Client-Server-chat/blob/master/images/3.png) <br>

* Messaging between multiple users. <br> Server on the left top. <br>
First of all user "John" creates room named "1". <br>
Other users open this room and start chatting with each other. <br>

![4](https://github.com/Haimasker/Client-Server-chat/blob/master/images/4.png) <br>
