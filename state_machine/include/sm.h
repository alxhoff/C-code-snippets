#ifndef __SM_H__
#define __SM_H__

unsigned int addState(void (*init)(void *), void (*enter)(void *),
		      void (*run)(void *), void (*exit)(void *), void *data);
void statesHandlerTask(void);

#endif
