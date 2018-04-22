for i in $(ps | grep main | grep -P '^\s*\d+' -o); do
	kill $i;
	echo killed $i;
done;
