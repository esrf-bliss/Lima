####################################################################################################################
# peut travailler en mode DEV   => copie des fichiers selon le profile local de l'utilisateur
# peut travailler en mode PROD  => copie des fichiers selon le profile DEVICE_ROOT
####################################################################################################################
set mode "unknown"

if {$::argc ==1} {
  set mode [lindex $::argv 0]
}

if {[string compare [string tolower $mode] "dev"]==0} {
  puts ""
  puts "======================================================================"
  puts "DEV mode : Files will be copied according to user profile"
  puts "======================================================================"
} else {
  if {[string compare $mode "prod"]==0} {
    puts ""
    puts "======================================================================"
    puts "PROD mode : Files will be copied according to DEVCICE_ROOT profile"
    puts "======================================================================"
  } else {
    puts ""
    puts "======================================================================"
    puts "UNKNOWN mode : Nothing To Do - Please specify DEV or PROD"
    puts "======================================================================"
  }
}


####################################################################################################################
#Rechercher un fichier par mot clef dans un repertoire et ses sous repertoires
####################################################################################################################
proc Find { ItemRecherche Chemin {Init 1}} {

  global nbFileTrouve 0
  global list_founded_files {}

  #si premier appel de Find par client, initialiser les variables
  if {$Init ==1} {
    set nbFileTrouve 0
    set list_founded_files {}
  }

  #on s'assure que le Chemin se termine par "/" au cas ou le client oublie de le preciser
  set chemin $Chemin
  if { [string index $chemin [expr [string length $chemin] - 1] ] != "/" } {
    set chemin $chemin/
  }


  #------------------------------------------------------------------------------------------------------------------
  #on decoupe en 2 parties pour pouvoir afficher traiter les dossiers d'abord, ensuite les fichiers
  #------------------------------------------------------------------------------------------------------------------
  if {[catch {set Listefile [glob $chemin*]}]==0} {

    #PARTIE 1 : Obtenir la liste de tous les dossiers
    foreach file $Listefile {
      if {[file isdirectory  $file]==1} {
        #recommencer de maniere recursive sur les sous dossiers
        Find $ItemRecherche $file/ 0
      }
    }


    #PARTIE 2 : Obtenir la liste de tous les fichiers
      foreach file $Listefile {
        if {[file isdirectory  $file]==0} {
          if {[regexp  -nocase $ItemRecherche [file tail $file]] !=0 } {
            incr nbFileTrouve
            #mettre à jour la liste des fichiers trouves
            lappend list_founded_files $file
          }
        }
      }
  }

  #on a tout trouve, on retourne la liste des fichiers trouves
  return $list_founded_files
}


####################################################################################################################
#Copier un fichier de Src vers Dest, en respectant l'arborescence de DEVICE_ROOT (plateformes : Unix, Windows)
#unix :
####################################################################################################################
proc copy_file {item Src Dest} {

    global To ""
    #si mode == DEV ou mode ==PROD
    if {[string compare [string tolower $::mode] "dev"]==0 || [string compare [string tolower $::mode] "prod"]==0} {

      #on s'assure que le chemin source se termine par "/" au cas ou le client oublie de le faire
      set src $Src
      if { [string index $src [expr [string length $src] - 1] ] != "/" } {
        set src $src/
      }

      #on s'assure que le chemin se termine par "/" au cas ou le client oublie de le faire
      set dest $Dest
      if { [string index $dest [expr [string length $dest] - 1] ] != "/" } {
        set dest $dest/
      }

      #chemin source relatif
      set From "[pwd]/$src"

      #définit ler chemin cible relatif selon la plateforme
      switch $::tcl_platform(platform) {
          unix {
            #si DEBUG, utiliser destination par rapport à /user comme sur triton
            if {[string compare [string tolower $::mode] "dev"]==0} {
              set To "/home/informatique/ica/$::tcl_platform(user)/DeviceServers"
            } else {
              set To "/usr/Local/DeviceServers/linux/$dest"
            }
          }
          windows {
            #si DEBUG, utiliser destination / user comme sur triton
            if {[string compare [string tolower $::mode] "dev"]==0} {
              set To "c:/DeviceServers"
            } else {
              set To "/usr/Local/DeviceServers/win32/$dest"
            }
          }
      }


      #Obtenir la liste de tous les fichiers
      set Liste_Items {}
      set Liste_Items [Find $item $src 1]

      if {[llength $Liste_Items] == 0} {
        puts "$item is not found in $From : ERROR"
      } else {
        foreach file $Liste_Items {
          if {[file isdirectory  $file]==0} {
              if {[catch {set TOTO [file copy -force $file  $To]} errText]} {
                puts "[file tail $file] : $errText"
              } else {
                puts "[file tail $file] : COPIED"
              }
          }
        }
      }
    }
}


#définit ler nom du fichier à copier selon la plateforme
switch $::tcl_platform(platform) {
    unix {

      #copy Device LimaDetector
      copy_file "ds_LimaDetector"           "applications/tango/LimaDetector/target/nar/"                       ""

      #copy lima.control library
      copy_file "libLimaControl"            "control/target/nar/lib"                                            "lib"

      #copy lima.common library
      copy_file "libLimaCommon"             "common/target/nar/lib"                                             "lib"

      #copy lima.hardware library
      copy_file "libLimaHardware"           "hardware/target/nar/lib"                                           "lib"

      #copy lima.third-partylibrary
      copy_file "libLimaProcesslibTasks"    "third-party/Processlib/tasks/target/nar/lib"                       "lib"

      #copy lima.third-partylibarary
      copy_file "libLimaProcesslibCore"     "third-party/Processlib/core/target/nar/lib"                        "lib"

      #copy lima.camera plugin library
      copy_file "libLimaBasler"             "camera/basler/target/nar/lib"                                      "lib"

      #copy lima.camera plugin library
      copy_file "libLimaSimulator"          "camera/simulator/target/nar/lib"                                   "lib"

      #copy lima.camera plugin library
      copy_file "libLimaXpad"               "camera/xpad/target/nar/lib"                                        "lib"

      #copy lima.camera plugin library
      copy_file "libLimaPilatus"            "camera/pilatus/target/nar/lib"                                     "lib"

      #copy lima.camera plugin library
      copy_file "libLimaMarccd"             "camera/marccd/target/nar/lib"                                    	"lib"
      
      #copy lima.camera plugin library
      copy_file "libLimaProsilica"          "camera/prosilica/target/nar/lib"                                   "lib"

      #copy lima.camera plugin library
      copy_file "libLimaAdsc"             	"camera/adsc/target/nar/lib"                                     	"lib"

      ################################# shared library delivered by providers ###################################
            
      copy_file "libauxlib.so"             	"camera/adsc/sdk/adsc_sources/lib/linux"                            "lib"      
      
      copy_file "libdetcon_th.so"           "camera/adsc/sdk/adsc_sources/lib/linux"                            "lib"
      
      copy_file "libPvAPI.so"             	"camera/prosilica/sdk/bin/x86"			                            "lib"
                  
    }

    windows {

      #copy Device LimaDetector
      copy_file "ds_LimaDetector"           "applications/tango/LimaDetector/target/nar/"               		""

      #copy lima.control library
      copy_file "libLimaControl"            "control/target/nar/lib"                                           	"lib"

      #copy lima.common library
      copy_file "libLimaCommon"             "common/target/nar/lib"                                             "lib"

      #copy lima.hardware library
      copy_file "libLimaHardware"           "hardware/target/nar/lib"                                           "lib"

      #copy lima.third-party library
      copy_file "libLimaProcesslibTasks"    "third-party/Processlib/nar/lib"                                    "lib"

      #copy lima.third-party library
      copy_file "libLimaProcesslibCore"     "third-party/Processlib/nar/lib"                                    "lib"

      #copy lima.camera plugin library
      copy_file "libLimaBasler"             "camera/basler/target/nar/lib"                                      "lib"

      #copy lima.camera plugin library
      copy_file "libLimaSimulator"          "camera/simulator/target/nar/lib"                                  	"lib"

      #copy lima.camera plugin library
      copy_file "libLimaXpad"               "camera/xpad/target/nar/lib"                                        "lib"

      #copy lima.camera plugin library
      copy_file "libLimaPilatus"            "camera/pilatus/target/nar/lib"                                    	"lib"

      #copy lima.camera plugin library
      copy_file "libLimaMarccd"             "camera/marccd/target/nar/lib"                                      "lib"
      
      #copy lima.camera plugin library
      copy_file "libLimaProsilica"          "camera/prosilica/target/nar/lib"                               	"lib"

      #copy lima.camera plugin library
      copy_file "libLimaAdsc"             	"camera/adsc/target/nar/lib"                                     	"lib"
      
      ################################# shared library delivered by providers ###################################
      
      copy_file "libauxlib.so"             	"camera/adsc/sdk/adsc_sources/lib/linux"                            "lib"      
      
      copy_file "libdetcon_th.so"           "camera/adsc/sdk/adsc_sources/lib/linux"	                        "lib"
      
      copy_file "libPvAPI.so"             	"camera/prosilica/sdk/bin/x86"				                        "lib"      
    }
}
