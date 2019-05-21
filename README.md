# ECC
export PATH=$PATH:$HOME/VelvetOptimiser-2.2.5
if [ -f /opt/rh/devtoolset-4/enable ]; then
        source scl_source enable devtoolset-4
fi

if [ -f /usr/local/bin/set_intel_xe_2016 ]; then
        source /usr/local/bin/set_intel_xe_2016
fi

if [ -f /usr/local/bin/intel_xe_2016 ]; then
        source /usr/local/bin/intel_xe_2016
fi

