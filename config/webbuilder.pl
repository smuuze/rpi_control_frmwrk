@rem = '
perl -w %0.bat
goto exit
';
defined @rem || die;

use Data::Dumper;
use Getopt::Long;          # Zur Einlesung der Optionen

#*******************************************************************************
#*******************************************************************************
# Programmbeginn
#*******************************************************************************
#*******************************************************************************

$opt_version = 0;
$opt_help = 0;

#===============================================================================
# Aufrufparameter laden
#===============================================================================
GetOptions(         # Optionsliste
      "in=s",       # Inputfile
      "out=s",      # Outputfile
      "sw=s",       # Schalter
      "help|h",     # Hilfe
      "version|v"); # version


#-------------------------------------------------------------------------------
# Version 0.3: Unterstützt Funktionen => "FUNCTION"

my $version = "webbuilder V0.3 \n";


#===============================================================================
# Versionsabfrage
#===============================================================================
if ($opt_version)
{
  print "$version";
  exit 0;
}

#===============================================================================
# Hilferuf
#===============================================================================
if ($opt_help)
{
  print "\n makefile Hilfe\n";
  print "----------------\n";
  print "     -h = Dieses Fenster\n";
  print "     -v = Version\n";
  print "     -in = Inputfile\n";
  print "     -out = Outputfile \n";
  print "     -sw = Schalter \n";
  print " \n";
  exit 0;
}




#===============================================================================
# Inpit und output Dateinamen erzeugen + globale Variablen.
#===============================================================================

my $out_file = 'out_file.txt';
my $in_file =  'in_file.txt';

my $path_prj = $0;      # Projektpfad besorgen
$path_prj =~ s/(\w+)\.pl$//i;

my $path_cfg;           # Pfad zur den Includfiles
my @source_files;       # Hier landen nur aktive include Dateien
my @all_source_files;   # Hier landen alle include Dateien
my @known_define;       # Hier werden die defines gesammelt

#-------------------------------------------------------------------------------
if ($opt_in) {
  $opt_in =~ s/\\/\//g;
  if ($opt_in =~ /\./) {                #ist ein '.' vorhanden?

    $in_file = $opt_in;
    $in_file =~ s/\.\w\w\w/.cfg/;
  }
  else {
    $in_file = "$opt_in.cfg";
  }
}
else {
  $in_file = "webconfig_road.cfg";
}

#-------------------------------------------------------------------------------
if ($opt_out) {
  if ($opt_out =~ /\./)                 #ist ein '.' vorhanden?
  {

    $out_file = $opt_out;
    $out_file =~ s/\.\w\w\w/.cfg/;

  }
  else {
    $out_file = "$opt_in.cfg";
  }
}
else {
   $out_file = "testweb.cfg";
}

#-------------------------------------------------------------------------------
if($opt_sw) {
  push(@known_define,$opt_sw);
}



#*******************************************************************************
# Hauptprogramm
#*******************************************************************************

#-------------------------------------------------------------------------------
  print "*******************************************************************\n";
  print "$version";
  print "*******************************************************************\n";
#-------------------------------------------------------------------------------

read_rodfile();   # Input Datei durchsuchen
build_cfg_file(); # Ausgabedatei zusammenstellen


#-------------------------------------------------------------------------------
 exit 0;




#*******************************************************************************
# Funktionen
#*******************************************************************************


#===============================================================================
# Extrahiert aus dem Road File
#     - Includefiles
#     - Defines
#     - Pfad zum Verzeichnis mit cfg Dateien, wenn vorhanden
#===============================================================================
sub read_rodfile
{
  print "-------------------------------------------------------------------\n";
  print " -- Parsing rod file\n";
  print "-------------------------------------------------------------------\n";
  print "      - Road file:       $in_file\n";
  print "      - Execution path:  $path_prj\n";


  my @define_level;

  open ROAD_FILE, "<$in_file" or die "can't read from $in_file";

  while($line=<ROAD_FILE>) {         #ROAD Datei bis zum Ende einlesen

    next if($line =~ /^\s*$/);       # Leerzeilen nicht auswerten
    next if($line =~ /^\s*\/\//);    # Kommentarzeilen nicht auswerten

    chomp $line;                     # CR/LF abtrennen

    if($line =~ /^\s*#PATH/) {       # PATH cmd suchen
      $path_cfg = $line;
      $path_cfg =~ s/\s*\#\w+\s*\=\s*//;  # Nur Pfad behalten
      $path_cfg =~ s/ +$//;               # Lehrzeichen am Ende abschneiden
      $path_cfg =~ s/\\$//;               # '\' am Ende entfernen, wenn vorhanden
      $path_cfg = "$path_cfg\\";          # '\' am Ende dranhängen
      #print "$path_cfg\n"; #-----TEST
    }
    elsif($line =~ /^\s*#define/) {  # gefundenen defines speichern.

      $temp = $line;
      $temp =~ s/\s*\#\w+\s*\s*//;
      push(@known_define,$temp);
    }
    elsif($line =~ /^\s*#ifdef/) {   # ifdef auswerten
      $temp = $line;
      $temp =~ s/\s*\#\w+\s*\s*//;
      push(@define_level,[$temp,1]);
    }
    elsif($line =~ /^\s*#ifndef/) {      # ifndef auswerten
      $temp = $line;
      $temp =~ s/\s*\#\w+\s*\s*//;
      push(@define_level,[$temp,2]);
    }
    elsif($line =~ /^\s*#endif/) {   # endif auswerten

      pop @define_level
    }
    elsif($line =~ /^\s*#include/) { # Includefiles sammeln
      $temp = $line;
      $temp =~ s/\s*\#\w+\s*\s*//;

      #--EK $level_count = 0;
      #--EK if(@define_level != 0) {       # Schauen ob die Datei includet werden soll
      #--EK
      #--EK   foreach $define_a (@define_level) {
      #--EK     foreach $define_b(@known_define) {
      #--EK       if($define_a =~ $define_b) {
      #--EK         $level_count++;
      #--EK         last;
      #--EK       }
      #--EK     }
      #--EK   }
      #--EK
      #--EK }

      $level_count = 0;
      if(@define_level != 0) {          # Schauen ob die Zeile aktiv ist
        foreach $define_a (@define_level) {
          $define_found = 0;
          foreach $define_b(@known_define) {
            #print("-> $define_a - $define_a->[0] - $define_a->[1] - @define_a\n"); #-----TEST
            if(($define_a->[0] =~ $define_b)) {
              $define_found = 1;
              last;
            }
          }

          if(($define_a->[1]==1)&&($define_found==1)){    # ifdef
            $level_count++;
          }
          elsif(($define_a->[1]==2)&&($define_found==0)){  # ifndef
            $level_count++;
          }
        }
      }

      if($level_count == @define_level ) {
        push(@source_files,[$temp]);   # Übergabe einer Referenz auf
                                       # ein anonymes Array [ $temp ].
                                       # Keine Ahnung wie, aber die einzelnen
                                       # Arrays bleiben erhalten.
        #print "$temp\n"; #-----TEST
      }
      push(@all_source_files,[$temp]); # Alle gefundenen includefiles speichern
    }
  }

  close ROAD_FILE;

  #------------------------------------------------------------------
  print "      - cfg path:        $path_cfg\n";
  print "      - Include files:\n";
  foreach $file (@source_files) {
    print "                         $file->[0]\n";
  }

  print "      - Defines:\n";

  foreach $define(@known_define) {
    print "                         $define\n";
  }


}


#===============================================================================
# Kopiert den Inhalt aus Dem Input File und den Includefiles zusammen.
#===============================================================================
sub build_cfg_file
{
  my @define_level = ();

  print "-------------------------------------------------------------------\n";
  print " -- Build cfg file\n";
  print "-------------------------------------------------------------------\n";
  print "      - Road file:       $in_file\n";
  print "      - Out file:        $out_file\n";

  open ROAD_FILE, "<$in_file" or die "can't read from $in_file";
  open OUTFILE,   ">$out_file" or die "can't open $out_file";
  $curant_part = "PART_CLR";

  while($line=<ROAD_FILE>) {   #ROAD Datei bis zum Ende einlesen

    if($line =~ /^\s*#PART_MASSAGE:/) {        # PART_MASSAGE einfügen
      $curant_part = "PART_MASSAGE";

      print OUTFILE "  //-------------------------------------------------------------------------\n";
      print OUTFILE "  // -- PART MASSAGE\n\n";
      get_blocks("MSG_VERSION", @all_source_files);   # MSG_VERSION Element einfuegen
      get_blocks("CMD_CONFIG", @all_source_files);    # CMD_CONFIG Elemente einfuegen
    }
    elsif($line =~ /^\s*#PART_FUNCTION:/) {            # PART_FUNKTION einfuegen
      $curant_part = "PART_FUNCTION";
      print OUTFILE "  //-------------------------------------------------------------------------\n";
      print OUTFILE "  // -- PART FUNCTION\n\n";
      get_blocks("FUNCTION", @source_files);     # CONFIG_OBJECT Elemente einfuegen
    }
    elsif($line =~ /^\s*#PART_OBJECT:/) {             # PART_OBJECT einfuegen
      $curant_part = "PART_OBJECT";
      print OUTFILE "  //-------------------------------------------------------------------------\n";
      print OUTFILE "  // -- PART OBJECT\n\n";
      get_blocks("CONFIG_OBJECT", @source_files);     # CONFIG_OBJECT Elemente einfuegen
    }
    elsif($line =~ /^\s*#PART_PAGE:/) {               # PART_PAGE einfuegen
      $curant_part = "PART_PAGE";
      print OUTFILE "  //-------------------------------------------------------------------------\n";
      print OUTFILE "  // -- PART PAGE\n\n";
    }
    elsif($line =~ /^\s*#PART_CLR:/) {
      $curant_part = "PART_CLR";                      # Partzuordnung loeschen
      print OUTFILE "------>$curant_part\n";
    }
    elsif($line =~ /^\s*#ifdef/) {                    # ifdef auswerten
      $temp = $line;
      $temp =~ s/\s*\#\w+\s*\s*//;
      push(@define_level,[$temp,1]);
    }
    elsif($line =~ /^\s*#ifndef/) {      # ifndef auswerten
      $temp = $line;
      $temp =~ s/\s*\#\w+\s*\s*//;
      push(@define_level,[$temp,2]);
    }
    elsif($line =~ /^\s*#endif/) {                    # endif auswerten
      pop @define_level
    }
    else{
      #--EK $level_count = 0;
      #--EK if(@define_level != 0) {       # Schauen ob die Zeile auskommentiert ist
      #--EK
      #--EK   foreach $define_a (@define_level) {
      #--EK     foreach $define_b(@known_define) {
      #--EK       if($define_a =~ $define_b) {
      #--EK         $level_count++;
      #--EK         last;
      #--EK       }
      #--EK     }
      #--EK   }
      #--EK }

      $level_count = 0;
      if(@define_level != 0) {          # Schauen ob die Zeile aktiv ist
        foreach $define_a (@define_level) {
          $define_found = 0;
          foreach $define_b(@known_define) {
            #print("-> $define_a - $define_a->[0] - $define_a->[1] - @define_a\n"); #-----TEST
            if(($define_a->[0] =~ $define_b)) {
              $define_found = 1;
              last;
            }
          }

          if(($define_a->[1]==1)&&($define_found==1)){    # ifdef
            $level_count++;
          }
          elsif(($define_a->[1]==2)&&($define_found==0)){  # ifndef
            $level_count++;
          }
        }
      }

      if($level_count == @define_level ) {

        if(!($line =~ /^\s*#/)) {   # Alle Zeile die nicht mit '#' anfangen kopieren
          print OUTFILE "$line";
        }
        else {
          if($curant_part =~ "PART_PAGE") {

            if($line =~ /^\s*#include/) {   # Includefile gefunden
              $temp = $line;
              $temp =~ s/\s*\#\w+\s*\s*//;

              #print "$temp\n"; #-----TEST

              @currant_file = ();            # Liste lehren
              push(@currant_file,[$temp]);
              get_blocks("SUMMARY_PAGE", @currant_file);
              get_blocks("PAGE", @currant_file);

            }
          }
        }
      }
    }
  }
  close ROAD_FILE;
  close OUTFILE;
}

#===============================================================================
# Durchsuch Includefiles nach dem angegeben Symbol und kopiert den Inhalt
# in die Ausgabedatei.
#
# z.B.:
# Suchsymbol  =  MSG_VERSION
#
#  MSG_VERSION                   |Dieser Bereich wird kopiert
#  {                             |Dieser Bereich wird kopiert
#    DWORD   vs_sernum;          |Dieser Bereich wird kopiert
#    WORD    vs_device_typ;      |Dieser Bereich wird kopiert
#    WORD    vs_sw_version;      |Dieser Bereich wird kopiert
#    WORD    vs_bl_version;      |Dieser Bereich wird kopiert
#    WORD    vs_hw_version;      |Dieser Bereich wird kopiert
#  }                             |Dieser Bereich wird kopiert
#
# Aufruf: get_blocks("CMD_CONFIG",@dateiliste)
#===============================================================================
sub get_blocks
{
  if(@_ < 1) {
    print "WARNUNG: Die Argumenten Liste der Funktion get_blocks ist falsch!";
    return 1;
  }
  my $klammerpaar = 0;
  my $block_found = 0;
  my $symbol = shift @_; # Suchstring => "CMD_CONFIG", "MSG_VERSION";
  my @file_list = @_;    # Dateiliste
  my @define_level;
  my $level_count;

  foreach $file (@file_list)
  {
    $sourcefile = "$path_cfg$file->[0]"; # Pfad+includefile

    #print "$sourcefile\n"; #-----TEST

    open  SOURCEFILE, "<$sourcefile" or die "can't open $sourcefile";

    @define_level = ();

    while($line=<SOURCEFILE>) {   #SOURCEFILE Datei bis zum Ende einlesen

      if($line =~ /^\s*#ifdef/) {         # ifdef auswerten
        $temp = $line;
        $temp =~ s/\s*\#\w+\s*\s*//;
        #--EK push(@define_level,$temp);
        push(@define_level,[$temp,1]);
      }
      elsif($line =~ /^\s*#ifndef/) {      # ifndef auswerten
        $temp = $line;
        $temp =~ s/\s*\#\w+\s*\s*//;
        push(@define_level,[$temp,2]);

      }
      elsif($line =~ /^\s*#endif/) {      # endif auswerten
        pop @define_level
      }
      else {

        $level_count = 0;
        if(@define_level != 0) {          # Schauen ob die Zeile aktiv ist
           foreach $define_a (@define_level) {
            $define_found = 0;
            foreach $define_b(@known_define) {
              # print("-> $define_a - $define_a->[0] - $define_a->[1] - @define_a\n"); #-----TEST
              #--EK if($define_a =~ $define_b) {
              if(($define_a->[0] =~ $define_b)) {
                $define_found = 1;
                last;
              }
            }

            if(($define_a->[1]==1)&&($define_found==1)){    # ifdef
              $level_count++;
            }
            elsif(($define_a->[1]==2)&&($define_found==0)){  # ifndef
              $level_count++;
            }
          }
        }

        if($level_count == @define_level ) {

          if($line =~ /^\s*$symbol/) {           # Blockanfang suchen
            print OUTFILE "$line";
            $block_found = 1;
            if($line =~ /\s*{/) {$klammerpaar++};
            if($line =~ /\s*}/) {
              $klammerpaar--;
              if($klammerpaar == 0){
                $block_found = 0;
                print OUTFILE "\n";
              }
            }
          }
          elsif($block_found == 1) {             # Blockinhalt kopieren
            print OUTFILE "$line";

            if($line =~ /\s*{/) {$klammerpaar++};
            if($line =~ /\s*}/) {$klammerpaar--};
            if($klammerpaar == 0){
              $block_found = 0;                  # Block ist zu ende
              print OUTFILE "\n";
            }
          }
        }
      }
    }
    close SOURCEFILE;
  }
  return 0;
}


#-------------------------------------------------------------------------------
__END__
:exit
