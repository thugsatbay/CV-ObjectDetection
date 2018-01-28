rm -rf part3-overfeat-features
mkdir part3-overfeat-features
if [ "$2" -gt 0 ]
then
./overfeat/bin/linux_64/overfeat_batch -l $2 -i $1 -o part3-overfeat-features 
else
./overfeat/bin/linux_64/overfeat_batch -i $1 -o part3-overfeat-features
fi
