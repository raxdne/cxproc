
# TODO Option image

ImageMagick

- use „VisualMagick configuration tool“ for VC++ Project setup on Windows
- Magic Scripting Language <http://www.linux-nantes.org/~fmonnier/ocaml/ml-conjure/documentation.html>
- Conjure <http://support.cs.nott.ac.uk/help/docs/image/ImageMagick/www/conjure.html>

- additional image processing instructions as elements
- embed comments into image files

    <cxp:image to="result.png">
      <cxp:image name="source.jpg" cache="yes"/>
      <cxp:scale="0.5"/>
      <cxp:orientation="auto"/>
      <cxp:comment>This is a Test!</cxp:comment>
      <cxp:scale="0.5"/>
      <cxp:scale="300"/>
    </cxp:image>

