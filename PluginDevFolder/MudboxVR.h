#include <QtGui/QColor>
#include <Mudbox/mudbox.h>
#include <MapExtractor/MapExtractorInterface.h>

class mudboxVR : public Node
{
    DECLARE_CLASS;
    virtual void OnNodeEvent( const Attribute &, NodeEventType );
}