/*********************************************************************************************************************
 * This file is part of 'yet Another Gamma Index Tool'.
 *
 * 'yet Another Gamma Index Tool' is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * 'yet Another Gamma Index Tool' is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 'yet Another Gamma Index Tool'; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *********************************************************************************************************************/

#include "dicom_tag_stategies.h"

#include <imebra/imebra.h>

using namespace imebra;
using namespace std;


// All DICOM tags from source DICOM are rewritten into result DICOM.
bool rewriteAllTagsStrategy(int, int, set <pair<int, int>>)
{
    return true;
}

// None of DICOM tags from source DICOM are rewritten into result DICOM.
bool rewriteNoTagsStrategy(int, int, set <pair<int, int>>)
{
    return false;
}

// Only non-private DICOM tags from source DICOM are rewritten into result DICOM.
bool rewriteAllNonPrivateTagsStrategy(int tagGroupId, int, set <pair<int, int>>)
{
    // DICOM Standard - Private attribute tags are tags with an odd group number greater than 8.
    return !(tagGroupId % 2 == 1 && tagGroupId > 8);
}

// DICOM tags from source DICOM are rewrited into result DICOM only if they are included in tagSet.
bool rewriteSpecifiedTagsStrategy(int tagGroupId, int tagTagId, set <pair<int, int>> tagSet)
{
    pair<int, int> tag = make_pair(tagGroupId, tagTagId);
    return tagSet.find(tag) != tagSet.end();
}

// All DICOM tags from source DICOM are rewrited into result DICOM except those that are included in tagSet.
bool omitSpecifiedTagsStrategy(int tagGroupId, int tagTagId, set <pair<int, int>> tagSet)
{
    pair<int, int> tag = make_pair(tagGroupId, tagTagId);
    return tagSet.find(tag) == tagSet.end();
}


// Function maps integer value passed by user to concrete tag rewrite strategy
// 5 - rewrite all tags from source dicom, except these included in function tagSet parameter
// 4 - rewrite only source dicom tags included in function tagSet parameter
// 3 - rewrite only not private tags from source dicom
// 2 - not rewrite any tags from source dicom
// 1 or other integer values - rewrite all tags from source dicom
tagsRewriteStrategy resolveTagsRewriteStrategy(int strategy)
{
    switch(strategy)
    {
        case 5:
            return omitSpecifiedTagsStrategy;
        case 4:
            return rewriteSpecifiedTagsStrategy;
        case 3:
            return rewriteAllNonPrivateTagsStrategy;
        case 2:
            return rewriteNoTagsStrategy;
        default:
            return rewriteAllTagsStrategy;
    }
}

void applyDicomTagsRewriteStrategy(DataSet* source, DataSet* destination, int rewriteTagsStrategyId, set <pair<int, int>> userTags)
{
    // Map strategy passed by parameter from integer value to pointer to function represent strategy
    tagsRewriteStrategy rewriteStrategy = resolveTagsRewriteStrategy(rewriteTagsStrategyId);

    // Get tags from source DICOM
    tagsIds_t tags = source->getTags();

    // Iterate over tags from source DICOM
    for (tagsIds_t::const_iterator scanTags = tags.begin(), endTags = tags.end(); scanTags != endTags; ++scanTags)
    {
        // Using selected strategy check, if tag (scanTags->getGroupId(), scanTags->getTagId()) should be rewritten into result DICOM
        if (rewriteStrategy(scanTags->getGroupId(), scanTags->getTagId(), userTags))
        {
            // 0x7fe0 = Pixel_Data tagId - we create new data so we shouldn't rewrite this tag from input dicom
            if (scanTags->getGroupId() == 0x7fe0)
                continue;
            try
            {
                destination->getTag(*scanTags);
            }
            catch (const MissingDataElementError &)
            {
                unique_ptr <Tag> sourceTag(source->getTag(*scanTags));
                unique_ptr <Tag> destTag(destination->getTagCreate(*scanTags, sourceTag->getDataType()));
                try
                {
                    for (size_t buffer = 0;; ++buffer)
                    {
                        try
                        {
                            unique_ptr <DataSet> sequence(sourceTag->getSequenceItem(buffer));
                            DataSet destSequence;
                            applyDicomTagsRewriteStrategy(sequence.get(), &destSequence, rewriteTagsStrategyId, userTags);
                            destination->setSequenceItem(*scanTags, buffer, destSequence);
                        }
                        catch (const MissingDataElementError &)
                        {
                            unique_ptr <ReadingDataHandler> sourceHandler(sourceTag->getReadingDataHandler(buffer));
                            unique_ptr <WritingDataHandler> destHandler(destTag->getWritingDataHandler(buffer));
                            destHandler->setSize(sourceHandler->getSize());
                            for (size_t item = 0; item != sourceHandler->getSize(); ++item)
                                destHandler->setUnicodeString(item, sourceHandler->getUnicodeString(item));
                        }
                    }
                }
                catch (const MissingDataElementError &) {}
            }
        }
    }
}
