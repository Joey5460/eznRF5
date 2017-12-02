#!/usr/bin/bash

#curl  -H "Authorization: token c8b4c4ddedf25745fdc8fb798ee984152668fc50 " -X POST https://api.github.com/repos/Joey5460/xsignal/releases -d '{"tag_name": "v0.0.1", "name": "0.0.1"}'

#curl  -H "Authorization: token c8b4c4ddedf25745fdc8fb798ee984152668fc50 " -X GET https://api.github.com/repos/bonetoday/Neo-Smart/releases
#curl  -H "Authorization: token c8b4c4ddedf25745fdc8fb798ee984152668fc50 " -X GET https://api.github.com/repos/Joey5460/xsignal/releases

#curl -H "Authorization: token c8b4c4ddedf25745fdc8fb798ee984152668fc50 " \
#     -H "Content-Type: application/zip" \
#     --data-binary '@a.zip ' \
#     -X POST "https://uploads.github.com/repos/Joey5460/xsignal/releases/4545143/assets?name=a.zip&size=521" 

GITHUBTOKEN=$(./jq -r .token release.json)
#TAG=$(./jq -r .tag release.json)
TAG=`git describe --abbrev=0 --tags`
echo $TAG
FILE=$(./jq -r .file release.json)
FILEHEX=$(./jq -r .filehex release.json)
REPO="bonetoday/Neo-Smart"
echo $GITHUBTOKEN
echo -n "Create draft release... "
echo .

JSON=$(cat <<EOF
{
  "tag_name":         "$TAG",
  "name":             "$TAG",
  "draft":            false,
  "prerelease":       true
}
EOF
)

RESULT=`curl \
  -H "Authorization: token $GITHUBTOKEN"  \
  -X POST \
  "https://api.github.com/repos/$REPO/releases" \
  -d "$JSON"`

RESULT=`curl \
  -H "Authorization: token $GITHUBTOKEN"  \
  -X GET \
  "https://api.github.com/repos/$REPO/releases/tags/$TAG"`

RELEASEID=`echo "$RESULT" | sed -ne 's/^  "id": \(.*\),$/\1/p'`
#RELEASEID=`echo "$RESULT" | sed -ne 's/^    "id": \(.*\),$/\1/p'`
echo "ReleaseID: $RELEASEID" 
if [[ -z "$RELEASEID" ]]; then
  echo Release FAILED
  exit 1
fi

FILESIZE=`stat -c '%s' "$FILE"`
FILENAME=`basename $FILE`
echo -n "Uploading $FILENAME... "

RESULT=`curl -H "Authorization: token $GITHUBTOKEN" \
     -H "Content-Type: application/zip" \
     --data-binary "@$FILE" \
     -X POST "https://uploads.github.com/repos/$REPO/releases/$RELEASEID/assets?name=$FILENAME&size=$FILESIZE"` 


FILESIZE=`stat -c '%s' "$FILEHEX"`
FILENAME=`basename $FILEHEX`
echo -n "Uploading $FILENAME... "

echo $(file -b --mime-type $FILEHEX)
RESULT=`curl -H "Authorization: token $GITHUBTOKEN" \
    -H "Content-Type: $(file -b --mime-type $FILEHEX)" \
     --data-binary "@$FILEHEX" \
     -X POST "https://uploads.github.com/repos/$REPO/releases/$RELEASEID/assets?name=$FILENAME&size=$FILESIZE"` 


echo DONE
