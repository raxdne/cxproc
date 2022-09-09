
# TODO Option “pie”

## Documentation

of the used Format

## Profiling

## XML

TODO: Update of RELAX NG schemas (translation by [trang](http://www.thaiopensource.com/relaxng/trang-manual.html) ) #v14

## Testing

Test with text files from Gutenberg Project <https://www.gutenberg.org/>
        <https://www.gutenberg.org/files/3176/3176-0.txt>

## Features

markup

       #begin_of_csv
       #import()
       <import type="csv" sep=",">A;B;C</import>
       <import name="a.csv" type="csv" sep=","/>
       <file name="a.csv" verbosity="4"/>

parse from UTF-8 memory buffer

REQ: handle https://en.wikipedia.org/wiki/ISO_8601#Week_dates ✔ 

REQ: append XML attributes to text markup
- `** TEST @assignee="abc" @date="1234567"`
- additional generic attributes as `[class="abc" cluster="cad" effort="1"]`
- JSON-like? `* Section json="[class:'abc', cluster:'cad', effort:1]"`

decoding of RFC1738 URL with Umlaute to UTF-8

TODO: change markup #v14

    <pre></pre>
    <script display="yes"></script>

REQ: add a default XSL reference `pie2html.xsl` to DOM ?

REQ: add meta element to every block element

REQ: define regexp for tags as node attribute `<pie tags="(ABC|XYZ)">` or  `<import tags="(ABC|XYZ)">` ✔ 

BUG: Tag recognition fails (end of local regexp) ✔

- regexp error handling??

Markdown

- <https://daringfireball.net/projects/markdown/basics>

- Markdown parser <https://css-tricks.com/choosing-right-markdown-parser/>

## References

### CSV

<https://github.com/dinedal/textql>

https://github.com/harelba/q

<https://stackoverflow.com/questions/12911299/read-csv-file-in-c>

<https://github.com/JamesRamm/csv_parser>

<https://github.com/rgamble/libcsv>


