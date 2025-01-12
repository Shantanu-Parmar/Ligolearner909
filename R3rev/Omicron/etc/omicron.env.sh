# source this file to access Omicron

##### GWOLLUM dependence
source /home/shan909/miniconda3/etc/gwollum.env.sh

##### PKG_CONFIG_PATH
PKG_CONFIG_PATH=`echo "$PKG_CONFIG_PATH" | /bin/sed -e 's|/home/shan909/Desktop/R3rev/Omicron/lib/pkgconfig:||g;'`
PKG_CONFIG_PATH="/home/shan909/Desktop/R3rev/Omicron/lib/pkgconfig:$PKG_CONFIG_PATH"
export PKG_CONFIG_PATH

##### PATH
PATH=`echo "$PATH" | /bin/sed -e 's|/home/shan909/Desktop/R3rev/Omicron/bin:||g;'`
PATH="/home/shan909/Desktop/R3rev/Omicron/bin:$PATH"
export PATH

##### LD_LIBRARY_PATH
LD_LIBRARY_PATH=`echo "$LD_LIBRARY_PATH " | /bin/sed -e 's|/home/shan909/Desktop/R3rev/Omicron/lib:||g;'`
LD_LIBRARY_PATH="/home/shan909/Desktop/R3rev/Omicron/lib:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH

##### Omicron environment
nodename=`uname -n`

OMICRON_HTML="/home/shan909/Desktop/R3rev/Omicron/share/omicron/html"
export OMICRON_HTML

# Lyon
if [[ $nodename == *"cc"* ]]; then
    OMICRON_TRIGGERS="/hpss/in2p3.fr/group/virgo/DETCHAR/triggers/Omicron"
    OMICRON_TRIGGERS_ONLINE="/sps/virgo/USERS/prod_virgo_omicron/triggers"

# Florent
elif [[ $nodename == "florent" ]]; then
    OMICRON_TRIGGERS="/home/florent/Analysis/Omicron/triggers"
    OMICRON_TRIGGERS_ONLINE="/home/florent/Analysis/Omicron/triggers/online"

# Cascina
elif [[ $nodename == *"virgo"* ]]; then
    OMICRON_TRIGGERS="/data/prod/omicron"
    OMICRON_TRIGGERS_ONLINE="/data/online/omicron"

# LIGO
elif [[ $nodename == *"ldas"* ]]; then
    OMICRON_TRIGGERS="/home/detchar/triggers"
    OMICRON_TRIGGERS_ONLINE=""

# unknown
else
    OMICRON_TRIGGERS=""
    OMICRON_TRIGGERS_ONLINE=""
fi
export OMICRON_TRIGGERS
export OMICRON_TRIGGERS_ONLINE
