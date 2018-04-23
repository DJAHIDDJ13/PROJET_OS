for i in $(ps | grep blackjack | grep -P '^\s*\d+' -o); do
	kill $i;
	echo killed $i;
done;
