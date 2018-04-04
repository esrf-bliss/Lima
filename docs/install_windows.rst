.. _windows:

Windows
=======

Prerequisite
````````````
Before installing Lima on windows plateform, you need to install the following tools :

	- `Python 2.6`_ or more recent.
	- `gsl-1.8`_.
	- `Visual C++ 2008 Express`_.
	- `GitHub`_ for windows.

	.. _Python 2.6: http://www.python.org/download/
	.. _gsl-1.8: http://sourceforge.net/projects/gnuwin32/files/gsl/1.8/gsl-1.8.exe/download?use_mirror=netcologne&download=
	.. _Visual C++ 2008 Express: http://www.microsoft.com/fr-fr/download/details.aspx?id=20682
	.. _GitHub: http://windows.github.com/

.. _windows_installation:

GetIt
`````
As Lima is not packaged,the only way for now is to retreived it from the git repository.
Lima can be retreived using **GitHub** for Windows. There are two ways to get Lima :

 - For ordinary user, you can get the source code directly (zip file),
 - For developpers who want to share their work with the community, you must create a fork and clone Lima.

The procedure to retreived is described below.

 - **create an account**

  *Launch GitHub form the desktop menu and click on "LOG IN".*
   .. image:: installation/GitHub_account_1.png
  *Click on "SIGN UP".*
   .. image:: installation/GitHub_account_2.png 
  *On the web page click on "Create a free account".*
   .. image:: installation/GitHub_account_3.png   
  *Answer the followning information and click on "Create an account".*
   .. image:: installation/GitHub_account_4.png   
   
 - **Get source code for ordinary user**

  *Launch GitHub form the desktop menu and click on "LOG IN".*
   .. image:: installation/GitHub_login_1.png
  *Enter your login and your pathword and click on "LOG IN".*
   .. image:: installation/GitHub_login_2.png
  *Click on your login to go to your dashboard.*
   .. image:: installation/GitHub_login_3.png
  *Once logged, enter "https://github.com/esrf-bliss/Lima" in the address bar.*
   .. image:: installation/GitHub_login_4.png

  *Retrieved source code by clicking on "ZIP button".*
  
 - **Fork and clone Lima for developpers**

  *Launch GitHub and log in. You can modify you default storage directory by selecting "Tools/Options..." menu.*
   .. image:: installation/GitHub_login_5.png

  *Repeat the previous steps and create a fork by clicking on "Fork". You can now clone Lima repository on your default storage directory.*

.. _windows_compilation:
  
Compilation & Installation
``````````````````````````
Installation of Lima works on Windows XP and Windows 7. 

Installation is done by following the steps below:
 - Copy config.inc_default to config.inc in the Lima directory.
 - Configure config.inc file for used cameras.

  .. image:: installation/config_inc.png

 - Run "install.bat" from the Lima directory. Installation takes a few minutes.

