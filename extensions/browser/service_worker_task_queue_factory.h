// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EXTENSIONS_BROWSER_SERVICE_WORKER_TASK_QUEUE_FACTORY_H_
#define EXTENSIONS_BROWSER_SERVICE_WORKER_TASK_QUEUE_FACTORY_H_

#include "base/macros.h"
#include "base/memory/singleton.h"
#include "components/keyed_service/content/browser_context_keyed_service_factory.h"

namespace extensions {

class ServiceWorkerTaskQueue;

class ServiceWorkerTaskQueueFactory : public BrowserContextKeyedServiceFactory {
 public:
  static ServiceWorkerTaskQueue* GetForBrowserContext(
      content::BrowserContext* context);
  static ServiceWorkerTaskQueueFactory* GetInstance();

 private:
  friend struct base::DefaultSingletonTraits<ServiceWorkerTaskQueueFactory>;

  ServiceWorkerTaskQueueFactory();
  ~ServiceWorkerTaskQueueFactory() override;

  // BrowserContextKeyedServiceFactory:
  KeyedService* BuildServiceInstanceFor(
      content::BrowserContext* context) const override;
  content::BrowserContext* GetBrowserContextToUse(
      content::BrowserContext* context) const override;

  DISALLOW_COPY_AND_ASSIGN(ServiceWorkerTaskQueueFactory);
};

}  // namespace extensions

#endif  // EXTENSIONS_BROWSER_SERVICE_WORKER_TASK_QUEUE_FACTORY_H_
