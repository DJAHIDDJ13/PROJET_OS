for i in $(ps | grep main | grep -P '\d{4}' -o); do
	kill $i;
	echo killed $i;
done;
