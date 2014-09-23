Musicdexer
==========

(Currently tailored for my own personal use) - This music database program allows you to add your own moods to songs. You can then search for songs by mood or any other category. The database features intelligent sync, automatic mp3 tag updating, quick advanced searches, and editing multiple songs at once.

==========

Usedl KDevelop4, QT Designer4, cmake, taglib(libtag1-dev), and MySQL. Table creation is listed at the bottom of musicdexer.cpp. I cannot guarantee the safety of your mp3 tag data so please replace /home/randy/Music/ with a testing directory. This appears 3 times in musicdexer.cpp and once in customqsqltablemodel.cpp. You also need to enter your database connection credentials(currently left blank) in musicdexer.cpp.
