@echo off 
copy webconfig_full_21BA0002_ProxEntry_Legic.dfg 21BA0002_00000000_00000000_00000000.cache
copy *.cache "%AppData%\deister electronic GmbH\DeisterConfig\Cache" /Y
del *.cache
