ls > a.txt | cat a.txt | wc -l

cat a.txt > b.txt | wc -l

wc -l < b.txt

wc -l < a.txt

ls

echo a.txt > c.txt

echo b.txt > d.txt

echo a.txt b.txt

echo c.txt; rm a.txt c.txt

echo d.txt; rm b.txt d.txt
