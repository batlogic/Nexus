
#include "asiodrvr.h"

AsioDriver::AsioDriver(LPUNKNOWN pUnk, HRESULT * phr){
}

AsioDriver::AsioDriver::AsioDriver(){
}

AsioDriver::AsioDriver::~AsioDriver(){
}

ASIOBool AsioDriver::AsioDriver::init(void * sysRef) {
}

void AsioDriver::AsioDriver::getDriverName(char * name) {
}

// max 32 bytes incl. terminating zero
long AsioDriver::AsioDriver::getDriverVersion() {
}

void AsioDriver::AsioDriver::getErrorMessage(char * string) {
}

// max 124 bytes incl.
ASIOError AsioDriver::AsioDriver::start() {
}

ASIOError AsioDriver::AsioDriver::stop() {
}

ASIOError AsioDriver::AsioDriver::getChannels(long * numInputChannels, long * numOutputChannels) {
}

ASIOError AsioDriver::AsioDriver::getLatencies(long * inputLatency, long * outputLatency) {
}

ASIOError AsioDriver::AsioDriver::getBufferSize(long * minSize, long * maxSize, long * preferredSize, long * granularity) {
}

ASIOError AsioDriver::AsioDriver::canSampleRate(ASIOSampleRate sampleRate) {
}

ASIOError AsioDriver::AsioDriver::getSampleRate(ASIOSampleRate * sampleRate) {
}

ASIOError AsioDriver::AsioDriver::setSampleRate(ASIOSampleRate sampleRate) {
}

ASIOError AsioDriver::AsioDriver::getClockSources(ASIOClockSource * clocks, long * numSources) {
}

ASIOError AsioDriver::AsioDriver::setClockSource(long reference) {
}

ASIOError AsioDriver::AsioDriver::getSamplePosition(ASIOSamples * sPos, ASIOTimeStamp * tStamp) {
}

ASIOError AsioDriver::AsioDriver::getChannelInfo(ASIOChannelInfo * info) {
}

ASIOError AsioDriver::AsioDriver::createBuffers(ASIOBufferInfo * bufferInfos, long numChannels, long bufferSize, ASIOCallbacks * callbacks) {
}

ASIOError AsioDriver::AsioDriver::disposeBuffers() {
}

ASIOError AsioDriver::AsioDriver::controlPanel() {
}

ASIOError AsioDriver::AsioDriver::future(long selector, void * opt) {
}

ASIOError AsioDriver::AsioDriver::outputReady() {
}

