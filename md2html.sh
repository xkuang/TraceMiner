# Use `pandoc` to convert a markdown file to HTML.
# I use it to convert the README wtc to HTML.
# Norman Dunbar.
# 22 February 2016

for x in *.md
do
   HTML="${x%%md}html"
   echo "Converting '${x}' to HTML as '${HTML}'"
   pandoc -f markdown -t html -o "${HTML}" "${x}"
done

